
#ifdef _WIN32
#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif
#include <GLFW/glfw3.h>

#include "scene.h"
#include "state.h"
#include "camera2d.h"
#include "color.h"
#include "transform.h"
#include "error.h"
#include "shader.h"

#include "orbit.h"

#include "disk.h"
#include "quad.h"

#include <iostream>

static ScenePtr scene;
static CameraPtr camera;

static void initialize (void)
{
  // set background color: black 
  glClearColor(0.f,.0f,.0f,1.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);

  // create objects
  camera = Camera2D::Make(0,30,0,30);

  auto center_trf = Transform::Make();
  auto mer_orbit_trf = Transform::Make();
  auto v_orbit_trf = Transform::Make();
  auto e_orbit_trf = Transform::Make();
  auto m_orbit_trf = Transform::Make();
  auto mar_orbit_trf = Transform::Make();

  auto sun_trf = Transform::Make();
  auto mercury_trf = Transform::Make();
  auto venus_trf = Transform::Make();
  auto earth_trf = Transform::Make();
  auto moon_trf = Transform::Make();
  auto mars_trf = Transform::Make();

  center_trf->Translate(15.f, 15.f, 0.f);

  mercury_trf->Translate(5.f, .0f, .0f);
  venus_trf->Translate(7.f, .0f, .0f);
  earth_trf->Translate(11.f, .0f, .0f);
  moon_trf->Translate(2.f, .0f, .0f);
  mars_trf->Translate(14.f, .0f, .0f);

  sun_trf->Scale(4.f, 4.f, 1.f);
  mercury_trf->Scale(.3f, .3f, 1.f);
  venus_trf->Scale(.8f, .8f, 1.f);
  // earth_trf->Scale(1.f, 1.f, 1.f);
  moon_trf->Scale(.2f, .2f, 1.f);
  mars_trf->Scale(.4f, .4f, 1.f);

  auto mars = Node::Make(mars_trf, { Color::Make(1,0,0) }, { Disk::Make() });
  auto mar_orbit = Node::Make(mar_orbit_trf, { mars });

  auto moon = Node::Make(moon_trf, { Color::Make(1,1,1) }, { Disk::Make() });
  auto m_orbit = Node::Make(m_orbit_trf, { moon });
  auto earth = Node::Make(earth_trf, { Color::Make(0,0,1) }, { Disk::Make() }, { m_orbit });
  auto e_orbit = Node::Make(e_orbit_trf, { earth });

  auto venus = Node::Make(venus_trf, { Color::Make(.65f,.24f,.16f) }, { Disk::Make() });
  auto v_orbit = Node::Make(v_orbit_trf, { venus });

  auto mercury = Node::Make(mercury_trf, { Color::Make(.79f,.65f,.16f) }, { Disk::Make() });
  auto mer_orbit = Node::Make(mer_orbit_trf, { mercury });

  auto sun = Node::Make(sun_trf, { Color::Make(1,1,0) }, { Disk::Make() });
  auto center = Node::Make(center_trf, { sun, mer_orbit, v_orbit, e_orbit , mar_orbit });

  auto shader = Shader::Make();
  shader->AttachVertexShader("./shaders/2d/vertex.glsl");
  shader->AttachFragmentShader("./shaders/2d/fragment.glsl");
  shader->Link();

  // build scene
  auto root = Node::Make(shader, { center });
  scene = Scene::Make(root);
  scene->AddEngine(Orbit::Make(mer_orbit_trf, 1.59f));
  scene->AddEngine(Orbit::Make(v_orbit_trf, 1.18f));
  scene->AddEngine(Orbit::Make(e_orbit_trf, 1.f));
  scene->AddEngine(Orbit::Make(m_orbit_trf, 13.f));
  scene->AddEngine(Orbit::Make(mar_orbit_trf, .81f));
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

static void update (float dt)
{
  scene->Update(dt);
}

int main ()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);       // required for mac os
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);  // option for mac os
#endif

    glfwSetErrorCallback(error);

    GLFWwindow* win = glfwCreateWindow(640, 480, "Window title", nullptr, nullptr);
    assert(win);
    glfwSetFramebufferSizeCallback(win, resize);  // resize callback
    glfwSetKeyCallback(win, keyboard);            // keyboard callback
    glfwMakeContextCurrent(win);
#ifdef _WIN32
    if (!gladLoadGL(glfwGetProcAddress)) {
        printf("Failed to initialize GLAD OpenGL context\n");
        exit(1);
    }
#endif
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  initialize();

  float t0 = float(glfwGetTime());
  while(!glfwWindowShouldClose(win)) {
    float t = float(glfwGetTime());
    update(t-t0);
    t0 = t;
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

