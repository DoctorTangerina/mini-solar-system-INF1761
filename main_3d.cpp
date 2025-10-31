
#ifdef _WIN32
#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif
#include <GLFW/glfw3.h>

#include "arcball.h"
#include "scene.h"
#include "state.h"
#include "camera3d.h"
#include "material.h"
#include "texture.h"
#include "transform.h"
#include "quad.h"
#include "sphere.h"
#include "error.h"
#include "shader.h"
#include "light.h"
#include "polyoffset.h"
#include "orbit.h"
#include "earth_camera_engine.h"
#include "cube.h"

#include <iostream>
#include <cassert>

static float viewer_pos[3] = {10.f, 5.f, 20.0f};

static ScenePtr scene;
static Camera3DPtr camera;
static Camera3DPtr camera_Earth;
static ArcballPtr arcball;

static bool in_earth_cam = false;

static void initialize(void)
{
    // set background color: black 
    glClearColor(0.f, .0f, .0f, 1.0f);

    // enable depth test 
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);  // cull back faces

    // create objects
    camera = Camera3D::Make(viewer_pos[0], viewer_pos[1], viewer_pos[2]);
    //camera->SetOrtho(true);
    arcball = camera->CreateArcball();

    camera_Earth = Camera3D::Make(9.f, 0.f, 0.f); // posição da Terra
    camera_Earth->SetCenter(11.f, 0.f, 0.f);      // direção da Lua
    camera_Earth->SetUpDir(0.f, 1.f, 0.f);
    camera_Earth->SetZPlanes(0.01f, 1000.f);

    //LightPtr light = ObjLight::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
    LightPtr light = Light::Make(0.0f, 0.0f, 0.0f, 1.0f, "world");

    // appearences
    AppearancePtr white = Material::Make(1.0f, 1.0f, 1.0f);
    AppearancePtr red = Material::Make(1.f, 0, 0);

    // diffuse textures
    AppearancePtr texSun = Texture::Make("decal", "./images/sun.jpg");
    AppearancePtr texMercury = Texture::Make("decal", "./images/mercury.png");
    AppearancePtr texVenus = Texture::Make("decal", "./images/venus.png");
    AppearancePtr texEarth = Texture::Make("decal", "./images/earth.jpg");
    AppearancePtr texMoon = Texture::Make("decal", "./images/moon.jpg");
    AppearancePtr texMars = Texture::Make("decal", "./images/mars.png");

    AppearancePtr texBg = Texture::Make("decal", "./images/space.jpg");


    // bump map
    AppearancePtr texMercuryNormal = Texture::Make("normal", "./images/mercury-normal.png");
    AppearancePtr texVenusNormal = Texture::Make("normal", "./images/venus-normal.png");
    AppearancePtr texEarthNormal = Texture::Make("normal", "./images/earth-normal.png");
    AppearancePtr texMoonNormal = Texture::Make("normal", "./images/moon-normal.jpg");
    AppearancePtr texMarsNormal = Texture::Make("normal", "./images/mars-normal.png");

    // create sun shader (without shadow)
    ShaderPtr shader_sun = Shader::Make(light, "world");
    shader_sun->AttachVertexShader("./shaders/ilum_vert/vertex_sun.glsl");
    shader_sun->AttachFragmentShader("./shaders/ilum_vert/fragment_sun.glsl");
    shader_sun->Link();
    
    // Define a different shader for texture mapping
    // An alternative would be to use only this shader with a "white" texture for untextured objects
    ShaderPtr shd_tex = Shader::Make(light, "world");
    shd_tex->AttachVertexShader("./shaders/ilum_vert/vertex_texture.glsl");
    shd_tex->AttachFragmentShader("./shaders/ilum_vert/fragment_texture.glsl");
    shd_tex->Link();

    Error::Check("before shps");
    ShapePtr sphere = Sphere::Make();
    Error::Check("after shps");
    ShapePtr cube = Cube::Make();
    Error::Check("after shps");

    // make transform
    auto bg_trf = Transform::Make();

    auto mer_orbit_trf = Transform::Make();
    auto v_orbit_trf = Transform::Make();
    auto e_orbit_trf = Transform::Make();
    auto m_orbit_trf = Transform::Make();
    auto mar_orbit_trf = Transform::Make();

    auto center_trf = Transform::Make();
    auto mer_center_trf = Transform::Make();
    auto v_center_trf = Transform::Make();
    auto e_center_trf = Transform::Make();
    auto m_center_trf = Transform::Make();
    auto mar_center_trf = Transform::Make();

    auto sun_trf = Transform::Make();
    auto mercury_trf = Transform::Make();
    auto venus_trf = Transform::Make();
    auto earth_trf = Transform::Make();
    auto moon_trf = Transform::Make();
    auto mars_trf = Transform::Make();

    // move objects
    mer_center_trf->Translate(3.f, 0.f, 0.f);
    v_center_trf->Translate(5.f, 0.f, 0.f);
    e_center_trf->Translate(9.f, 0.f, 0.f);
    m_center_trf->Translate(2.f, 0.f, 0.f);
    mar_center_trf->Translate(13.f, 0.f, 0.f);
    bg_trf->Translate(0.f, -15.f, 0.f);

    sun_trf->Scale(2.f, 2.f, 2.f);
    mercury_trf->Scale(.3f, .3f, .3f);
    venus_trf->Scale(.8f, .8f, .8f);
    // earth_trf->Scale(1.f, 1.f, 1.f);
    moon_trf->Scale(.2f, .2f, .2f);
    mars_trf->Scale(.4f, .4f, .4f);

    bg_trf->Scale(50.f, 50.f, 50.f);

    // build scene

    auto bg = Node::Make(shader_sun, bg_trf, { white, texBg }, { cube });

    auto mars = Node::Make(mars_trf, { white, texMars, texMarsNormal }, {sphere});
    auto mar_center = Node::Make(mar_center_trf, { mars });
    auto mar_orbit = Node::Make(mar_orbit_trf, { mar_center });

    auto moon = Node::Make(moon_trf, { white, texMoon, texMoonNormal }, { sphere });
    auto earth = Node::Make(earth_trf, { white, texEarth, texEarthNormal }, { sphere });
    auto m_center = Node::Make(m_center_trf, { moon });
    auto m_orbit = Node::Make(m_orbit_trf, { m_center });
    auto e_center = Node::Make(e_center_trf, { earth,  m_orbit });
    auto e_orbit = Node::Make(e_orbit_trf, { e_center });

    auto venus = Node::Make(venus_trf, { white, texVenus, texVenusNormal }, { sphere });
    auto v_center = Node::Make(v_center_trf, { venus });
    auto v_orbit = Node::Make(v_orbit_trf, { v_center });

    auto mercury = Node::Make(mercury_trf, { white, texMercury, texMercuryNormal }, { sphere });
    auto mer_center = Node::Make(mer_center_trf, { mercury });
    auto mer_orbit = Node::Make(mer_orbit_trf, { mer_center });

    auto sun = Node::Make(shader_sun, sun_trf, { texSun }, { sphere });
    auto center = Node::Make(center_trf, { sun, mer_orbit, v_orbit, e_orbit , mar_orbit});

    NodePtr root = Node::Make(shd_tex, { center, bg });
    scene = Scene::Make(root);

    //translate
    scene->AddEngine(Orbit::Make(mer_orbit_trf, 1.59f));
    scene->AddEngine(Orbit::Make(v_orbit_trf, 1.18f));
    scene->AddEngine(Orbit::Make(e_orbit_trf, 1.f));
    scene->AddEngine(Orbit::Make(m_orbit_trf, 13.f));
    scene->AddEngine(Orbit::Make(mar_orbit_trf, .81f));

    //rotation
    scene->AddEngine(Orbit::Make(sun_trf, 5.f));
    scene->AddEngine(Orbit::Make(mercury_trf, 45.25f));
    scene->AddEngine(Orbit::Make(venus_trf, -30.f));
    scene->AddEngine(Orbit::Make(earth_trf, 60.f));
    //scene->AddEngine(Orbit::Make(moon_trf, 13.f));
    scene->AddEngine(Orbit::Make(mars_trf, 59.f));
    scene->AddEngine(Earth_camera_engine::Make(earth_trf, 60.f));

}

static void display (GLFWwindow* win)
{ 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window 
  Error::Check("before render");
  scene->Render(in_earth_cam ? camera_Earth : camera);
  Error::Check("after render");
}

static void error (int code, const char* msg)
{
  printf("GLFW error %d: %s\n", code, msg);
  glfwTerminate();
  exit(0);
}

static void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  if (key == GLFW_KEY_C && action == GLFW_PRESS)
    in_earth_cam = !in_earth_cam;
}

static void resize (GLFWwindow* win, int width, int height)
{
  glViewport(0,0,width,height);
}

static void cursorpos (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->AccumulateMouseMotion(int(x),int(y));
}
static void cursorinit (GLFWwindow* win, double x, double y)
{
  // convert scre,en pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->InitMouseMotion(int(x),int(y));
  glfwSetCursorPosCallback(win, cursorpos);     // cursor position callback
}
static void mousebutton (GLFWwindow* win, int button, int action, int mods)
{
  if (action == GLFW_PRESS) {
    glfwSetCursorPosCallback(win, cursorinit);     // cursor position callback
  }
  else // GLFW_RELEASE 
    glfwSetCursorPosCallback(win, nullptr);      // callback disabled
}

static void update(float dt)
{
    scene->Update(dt);

}

int main ()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);       // required for mac os
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER,GLFW_TRUE);  // option for mac os
#endif

  glfwSetErrorCallback(error);

  GLFWwindow* win = glfwCreateWindow(600,400,"Window title",nullptr,nullptr);
  assert(win);
  glfwSetFramebufferSizeCallback(win, resize);  // resize callback
  glfwSetKeyCallback(win, keyboard);            // keyboard callback
  glfwSetMouseButtonCallback(win, mousebutton); // mouse button callback
  
  glfwMakeContextCurrent(win);
#ifdef _WIN32
  if (!gladLoadGL()) {
      printf("Failed to initialize GLAD OpenGL context\n");
      exit(1);
  }
#endif
  printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  initialize();

  float t0 = float(glfwGetTime());
  while(!glfwWindowShouldClose(win)) {
    float t = float(glfwGetTime());
    update(t - t0);
    t0 = t;
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

