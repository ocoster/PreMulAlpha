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
  void updatePFX(float i_delta);

protected:
  
  enum class ParticleType : uint8_t
  {
    Additive = 0,
    Multiply,
    Blend,

    MAX
  };


  struct Particle
  {
    inline Particle() { Reset(); }
    void Reset();

    vec2 m_position = vec2(0.0f, 0.0f);
    float m_alpha = 0.0f;
    float m_size = 0.0f;
    float m_rotation = 0.0f;
    ParticleType m_type = ParticleType::Additive;

    vec2 m_direction = vec2(0.0f, 1.0f);
    float m_alphaDelta = 0.1f;
    float m_sizeDelta = 0.0f;
    float m_rotationDelta = 0.0f;

  };


  mat4 m_projection; //!< The projection matrix used
  SamplerStateID trilinearClamp, trilinearAniso, radialFilter;

  int32_t m_divPos = 0;

  std::vector<Particle> m_particles; //!< The array of rendre particles

  TextureID m_texBackground; 

  TextureID m_texAdditve;
  TextureID m_texMultiply;
  TextureID m_texBlend;

  BlendStateID m_blendModeAdditve;
  BlendStateID m_blendModeMultiply;
  BlendStateID m_blendModeBlend;

  TextureID m_texPreMul;
  BlendStateID m_blendModePreMul;

  //ShaderID m_gridDraw;
}; 
