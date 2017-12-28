//======================================================================
// BlendDemo
//      By Damian Trebilco
//======================================================================

#include "App.h"
#include <crtdbg.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> 
#include <algorithm>
#include <time.h>

extern long g_allocRequestCount;
extern long g_allocFreeCount;

BaseApp *CreateApp() { return new App(); }

bool s_mouseLeftDown = false;


float RandomFloat(float i_min, float i_max)
{
  float random = ((float)rand()) / (float)RAND_MAX;

  float range = i_max - i_min;
  return (random * range) + i_min;
}

void App::Particle::Reset()
{
  m_position = vec2(0.0f, 0.0f);
  m_alpha = 1.0f;
  m_size = 1.0f;
  m_rotation = 0.0f;
  m_texType = 0;

  m_direction = vec2(RandomFloat(-2.0f, 2.0f), RandomFloat(2.0f, 3.0f));
  m_alphaDelta = RandomFloat(-0.1f, -0.05f);

}

App::App()
{
}

bool App::init()
{
  m_particles.resize(300);

  // Age the pfx system for the first draw
  for (uint32_t i = 0; i < 1000; i++)
  {
    updatePFX(1.0f / 30.0f);
  }

  return OpenGLApp::init();
}

bool App::onKey(const uint key, const bool pressed)
{
  if (pressed)
  {
    //switch (key)
    //{
    //case '1': break;
    //case '2': break;
    //case '3': break;
    //}
  }
  return BaseApp::onKey(key, pressed);
}

bool App::load()
{
  // Set the shader version used
  ((OpenGLRenderer*)renderer)->SetShaderVersionStr("#version 130");

  // Filtering modes
  if ((trilinearClamp = renderer->addSamplerState(TRILINEAR, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;
  if ((trilinearAniso = renderer->addSamplerState(TRILINEAR_ANISO, WRAP, WRAP, WRAP)) == SS_NONE) return false;
  if ((radialFilter   = renderer->addSamplerState(LINEAR, WRAP, CLAMP, CLAMP)) == SS_NONE) return false;

  //if ((m_perlin = renderer->addTexture("Perlin.png", true, trilinearAniso)) == TEXTURE_NONE) return false;
  //if ((m_gridDraw = renderer->addShader("gridDraw.shd")) == SHADER_NONE) return false;


  m_divPos = width / 2;

  return true;
}

bool App::onMouseButton(const int x, const int y, const MouseButton button, const bool pressed)
{
  if (button == MOUSE_LEFT)
  {
    s_mouseLeftDown = pressed;
    m_divPos = x;
  }
  return OpenGLApp::onMouseButton(x, y, button, pressed);
}

bool App::onMouseMove(const int x, const int y, const int deltaX, const int deltaY)
{
  if (s_mouseLeftDown)
  {
    m_divPos = x;
  }
  return OpenGLApp::onMouseMove(x, y, deltaX, deltaY);
}

void DrawRoom(uint32_t a_w, uint32_t a_h, uint32_t a_pixelSize)
{
  glColor3f(0.0f, 1.0f, 0.0f);

  glBegin(GL_LINES);
  for (uint32_t i = 0; i <= a_w; i++)
  {
    glVertex2i(i * a_pixelSize, 0);
    glVertex2i(i * a_pixelSize, a_h * a_pixelSize);
  }
  for (uint32_t i = 0; i <= a_h; i++)
  {
    glVertex2i(0, i * a_pixelSize);
    glVertex2i(a_w * a_pixelSize, i * a_pixelSize);
  }
  glEnd();
}

void FillBlockPixel(uint32_t a_x, uint32_t a_y, uint32_t a_pixelSize)
{
  glVertex2i(a_x, a_y);
  glVertex2i(a_x + a_pixelSize, a_y);

  glVertex2i(a_x + a_pixelSize, a_y + a_pixelSize);
  glVertex2i(a_x, a_y + a_pixelSize);
}

void App::updatePFX(float i_delta)
{
  // Update all particles and reset ones that expire
  for (Particle& p : m_particles)
  {
    p.m_position += p.m_direction * i_delta;
    p.m_alpha += p.m_alphaDelta * i_delta;

    if (p.m_alpha <= 0.0f)
    {
      p.Reset();
    }
  }
}

void App::drawFrame()
{
  // Update the PFX
  updatePFX(frameTime);

  mat4 modelview = scale(1.0f, 1.0f, -1.0f) * rotateXY(-wx, -wy) * translate(-camPos) * rotateX(PI * 0.5f);

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(value_ptr(m_projection));

  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(value_ptr(modelview));

  float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  renderer->clear(true, true, false, clearColor);


  // Draw the background

  // Draw the pre-mul alpha
    // Setup scissor

  // Draw the traditional blending
    // Setup scissor


  // Reset the scissor

  renderer->reset();
  renderer->setup2DMode(-50.0f, 50.0f, (float)height / (float)width * 100.0f, 0.0f);
  renderer->apply();
  glBegin(GL_QUADS);
  glColor3f(1.0f, 1.0f, 0.0f);
  for (Particle& p : m_particles)
  {
    glVertex2f(p.m_position.x - p.m_size, p.m_position.y - p.m_size);
    glVertex2f(p.m_position.x + p.m_size, p.m_position.y - p.m_size);
    glVertex2f(p.m_position.x + p.m_size, p.m_position.y + p.m_size);
    glVertex2f(p.m_position.x - p.m_size, p.m_position.y + p.m_size);
  }
  glEnd();

  // Draw the dividing line
  renderer->reset();
  renderer->setup2DMode(0, (float)width, 0, (float)height);
  renderer->apply();
  glBegin(GL_QUADS);
    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex2i(m_divPos - 2, 0);
    glVertex2i(m_divPos + 4, 0);
    glVertex2i(m_divPos + 4, height);
    glVertex2i(m_divPos - 2, height);
  glEnd();

  // Draw the draw call counts


  renderer->reset();
  //renderer->setShader(m_gridDraw);
  //renderer->setTexture("perlinTex", m_perlin);
  renderer->apply();

  renderer->reset();
  renderer->setDepthState(noDepthWrite);
  renderer->apply();

  {
    char buffer[100];
    float xPos = (float)width - 250.0f;

#ifdef _DEBUG
    sprintf(buffer, "Alloc Count %d", g_allocRequestCount);
    renderer->drawText(buffer, xPos, 138.0f, 30, 38, defaultFont, linearClamp, blendSrcAlpha, noDepthTest);
    
    sprintf(buffer, "Free Count %d", g_allocFreeCount);
    renderer->drawText(buffer, xPos, 168.0f, 30, 38, defaultFont, linearClamp, blendSrcAlpha, noDepthTest);
    
    sprintf(buffer, "Working Count %d", g_allocRequestCount - g_allocFreeCount);
    renderer->drawText(buffer, xPos, 198.0f, 30, 38, defaultFont, linearClamp, blendSrcAlpha, noDepthTest);
#endif // _DEBUG
  }
}
