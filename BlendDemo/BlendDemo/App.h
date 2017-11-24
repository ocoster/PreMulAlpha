//======================================================================
// BlendDemo
//      By Damian Trebilco
//======================================================================

#include "../Framework3/OpenGL/OpenGLApp.h"
#include <vector>

class App : public OpenGLApp {
public:

  App();

  char *getTitle() const override { return "Blend Demo"; }
  bool init() override;

  bool onKey(const uint key, const bool pressed) override;
  bool onMouseButton(const int x, const int y, const MouseButton button, const bool pressed) override;
  bool onMouseMove(const int x, const int y, const int deltaX, const int deltaY) override;

  bool load() override;

  void drawFrame() override;

protected:

  struct Particle
  {
    vec2 m_position = vec2(0.0f, 0.0f);
    vec2 m_direction = vec2(0.0f, 1.0f);
    float m_alpha = 0.0f;
    float m_size = 0.0f;
    float m_rotation = 0.0f;
    uint8_t m_texType = 0;
  };


  mat4 m_projection; //!< The projection matrix used
  SamplerStateID trilinearClamp, trilinearAniso, radialFilter;

  int32_t m_divPos = 0;

  std::vector<Particle> m_particles; //!< The array of rendre particles

  //TextureID m_perlin; 
  //ShaderID m_gridDraw;
}; 
