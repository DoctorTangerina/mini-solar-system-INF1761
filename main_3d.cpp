
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

#include <iostream>
#include <cassert>

static float viewer_pos[3] = {2.0f, 3.5f, 4.0f};

static ScenePtr scene;
static Camera3DPtr camera;
static ArcballPtr arcball;

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

    //LightPtr light = ObjLight::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
    LightPtr light = Light::Make(0.0f, 0.0f, 0.0f, 1.0f, "camera");

    // appearences
    AppearancePtr white = Material::Make(1.0f, 1.0f, 1.0f);
    /*AppearancePtr texSpace = Texture::Make("decal", "./images/space.jpg");
    AppearancePtr texSun = Texture::Make("decal", "./images/sun.jpg");
    AppearancePtr texMercury = Texture::Make("decal", "./images/mercury.png");
    AppearancePtr texVenus = Texture::Make("decal", "./images/venus.png");*/
    AppearancePtr texEarth = Texture::Make("decal", "./images/earth.jpg");
    /*AppearancePtr texMoon = Texture::Make("decal", "./images/moon.png");
    AppearancePtr texMars = Texture::Make("decal", "./images/mars.png");*/

    // create shader
    ShaderPtr shader = Shader::Make(light, "camera");
    shader->AttachVertexShader("./shaders/ilum_vert/vertex.glsl");
    shader->AttachFragmentShader("./shaders/ilum_vert/fragment.glsl");
    shader->Link();

    // Define a different shader for texture mapping
    // An alternative would be to use only this shader with a "white" texture for untextured objects
    ShaderPtr shd_tex = Shader::Make(light, "camera");
    shd_tex->AttachVertexShader("./shaders/ilum_vert/vertex_texture.glsl");
    shd_tex->AttachFragmentShader("./shaders/ilum_vert/fragment_texture.glsl");
    shd_tex->Link();

    Error::Check("before shps");
    ShapePtr sphere = Sphere::Make();
    Error::Check("after shps");

    //transform
    auto bg_trf = Transform::Make();
    auto center_trf = Transform::Make();
    auto mer_orbit_trf = Transform::Make();
    auto v_orbit_trf = Transform::Make();
    auto e_orbit_trf = Transform::Make();
    auto m_orbit_trf = Transform::Make();
    auto mar_orbit_trf = Transform::Make();

    auto earth_moon_trf = Transform::Make();

    auto sun_trf = Transform::Make();
    auto mercury_trf = Transform::Make();
    auto venus_trf = Transform::Make();
    auto earth_trf = Transform::Make();
    auto moon_trf = Transform::Make();
    auto mars_trf = Transform::Make();

    // build scene

    auto earth = Node::Make(earth_trf, { white, texEarth }, {sphere});
    auto center = Node::Make(center_trf, { earth });

    NodePtr root = Node::Make(shd_tex, { center });
    scene = Scene::Make(root);
}

static void display (GLFWwindow* win)
{ 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window 
  Error::Check("before render");
  scene->Render(camera);
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
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
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
  if (!gladLoadGL(glfwGetProcAddress)) {
      printf("Failed to initialize GLAD OpenGL context\n");
      exit(1);
  }
#endif
  printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  initialize();

  while(!glfwWindowShouldClose(win)) {
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

