
#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

enum class ImageMode
{
  Additive,
  AlphaBlend,
  Multiply,
  PreMulAlpha,
};

void PrintUsage()
{
  printf("Usage: BlendProcess.exe <input blend> <input file> outputfile.png\n");
  printf("Input blend can specify one of the folowing:\n");
  printf(" \"-a\" - input is additive\n");
  printf(" \"-b\" - input is alpha blend\n");
  printf(" \"-m\" - input is multiply (red channel)\n");
  printf(" \"-p\" - input is pre-mul alpha\n\n");
  printf("eg. BlendProcess.exe -a base.png -b blend.png out.png\n\n");
  printf("To add more layers: BlendProcess.exe -p out.png -b new.png out.png\n");
  printf("Input file can be of several formats, but output is always png.\n");
}

bool GetImageMode(const char* a_mode, ImageMode& a_retMode)
{
  if (strcmp(a_mode, "-a") == 0) { a_retMode = ImageMode::Additive;    return true; }
  if (strcmp(a_mode, "-b") == 0) { a_retMode = ImageMode::AlphaBlend;  return true; }
  if (strcmp(a_mode, "-m") == 0) { a_retMode = ImageMode::Multiply;    return true; }
  if (strcmp(a_mode, "-p") == 0) { a_retMode = ImageMode::PreMulAlpha; return true; }
  return false;
}

float SRGBToLinear(float a_col)
{
  if (a_col <= 0.04045f)
  {
    return a_col / 12.92f;
  }
  return powf((a_col + 0.055f) / 1.055f, 2.4f);
}

float LinearToSRGB(float a_col)
{
  if (a_col <= 0.0031308f)
  {
    return 12.92f * a_col;
  }

  return 1.055f * powf(a_col, 1.0f / 2.4f) - 0.055f;
}

template <class T>
inline T clamp(const T &a_value,  const T &a_min, const T &a_max)
{
  return (a_value < a_min) ? a_min : (a_value > a_max) ? a_max : a_value;
}

int main(int a_argc, char* a_argv[])
{
  // If not enough args, print help
  if (a_argc < 4)
  {
    PrintUsage();
    return 1;
  }

  uint32_t imageWidth = 0;
  uint32_t imageHeight = 0;
  std::vector<float> imageData;

  // Loop for all options
  int32_t currArg = 1;
  while ((currArg  + 2) < a_argc)
  {
    // Get the option
    ImageMode mode = ImageMode::Additive;
    if (!GetImageMode(a_argv[currArg], mode))
    {
      printf("Invalid image mode (%s)\n", a_argv[currArg]);
      PrintUsage();
      return 1;
    }

    // Read the file
    int x = 0, y = 0, n = 0;
    const char * filename = a_argv[currArg + 1];
    unsigned char *data = stbi_load(filename, &x, &y, &n, 4);
    if (data == nullptr ||
        x <= 0 ||
        y <= 0)
    {
      printf("Image file (%s) is invalid\n", filename);
      return 1;
    }

    // Check if the image mode makes sense fo the image data
    if (n < 4 &&
        mode != ImageMode::Additive)
    {
      printf("Image (%s) did not contain an alpha - probably wrong processing\n", filename);
    }

    // If the first file, setup the buffer
    if (currArg == 1)
    {
      imageWidth = x;
      imageHeight = y;
      imageData.resize(imageWidth * imageHeight * 4);
      for (uint32_t i = 0; i < (imageWidth * imageHeight); i++)
      {
        imageData[i * 4 + 0] = 0.0f;
        imageData[i * 4 + 1] = 0.0f;
        imageData[i * 4 + 2] = 0.0f;
        imageData[i * 4 + 3] = 1.0f;
      }
    }
    // If not the first file, check that the dimensions are valid
    else if(imageWidth != uint32_t(x) ||
            imageHeight != uint32_t(y))
    {
      printf("Image file (%s) has dimensions (%d,%d) - was expecting (%u,%u)\n", filename, x, y, imageWidth, imageHeight);
      return 1;
    }

    // Convert to floats and to the pre-mul format
    for (uint32_t i = 0; i < (imageWidth * imageHeight); i++)
    {
      float color[4] = {
        SRGBToLinear(float(data[i * 4 + 0]) / 255.0f),
        SRGBToLinear(float(data[i * 4 + 1]) / 255.0f),
        SRGBToLinear(float(data[i * 4 + 2]) / 255.0f),
        float(data[i * 4 + 3]) / 255.0f,
      };

      // Convert to pre-mul alpha
      switch (mode)
      {
        case(ImageMode::Additive) : 
          color[3] = 0.0f; 
          break;

        case(ImageMode::AlphaBlend) : 
          color[0] *= color[3];
          color[1] *= color[3];
          color[2] *= color[3];
          break;

        case(ImageMode::Multiply) : 
          color[3] = 1.0f - color[0];
          color[0] = 0.0f;
          color[1] = 0.0f;
          color[2] = 0.0f;
          break;
      }

      // Apply to the pixel
      float invAlpha = 1.0f - color[3];
      float* dstPixel = &imageData[i * 4];
      dstPixel[0] = dstPixel[0] * invAlpha + color[0];
      dstPixel[1] = dstPixel[1] * invAlpha + color[1];
      dstPixel[2] = dstPixel[2] * invAlpha + color[2];
      dstPixel[3] = dstPixel[3] * invAlpha;
    }

    stbi_image_free(data);
    currArg += 2;
  } 

  // Convert the final floats to sRGB space
  std::vector<uint8_t> outputData;
  outputData.resize(imageData.size());
  for (uint32_t i = 0; i < (imageWidth * imageHeight); i++)
  {
    const float* srcPixel = &imageData[i * 4];
    uint8_t* dstPixel = &outputData[i * 4];
    dstPixel[0] = (uint8_t)(clamp(LinearToSRGB(srcPixel[0]), 0.0f, 1.0f) * 255.0f);
    dstPixel[1] = (uint8_t)(clamp(LinearToSRGB(srcPixel[1]), 0.0f, 1.0f) * 255.0f);
    dstPixel[2] = (uint8_t)(clamp(LinearToSRGB(srcPixel[2]), 0.0f, 1.0f) * 255.0f);
    dstPixel[3] = (uint8_t)(clamp(1.0f - srcPixel[3], 0.0f, 1.0f) * 255.0f);
  }

  // Write output png file
  const char * outFilename = a_argv[currArg];
  if (stbi_write_png(outFilename, imageWidth, imageHeight, 4, outputData.data(), imageWidth * 4) == 0)
  {
    printf("Failure to write output image (%s)\n", outFilename);
    return 1;
  }

  return 0;
}
 
