#ifdef _WIN32
#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "arcball.h"
#include "scene.h"
#include "state.h"
#include "camera3d.h"
#include "material.h"
#include "texture.h"
#include "transform.h"
#include "cube.h"
#include "quad.h"
#include "sphere.h"
#include "error.h"
#include "shader.h"
#include "light.h"
#include "polyoffset.h"
#include "variable.h"
#include "framebuffer.h"
#include "texdepth.h"

#include <iostream>
#include <cassert>

#define DIM 1024

static float viewer_pos[3] = {2.0f, 3.5f, 4.0f};

static ScenePtr scene;
static Camera3DPtr camera, shadow_camera;
static ArcballPtr arcball;
static FramebufferPtr fbo;
static ShaderPtr shd_tex, shader_sm;

static void initialize (void)
{
  // set background color: white 
  glClearColor(1.0f,1.0f,1.0f,1.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);  // cull back faces

  // create objects
  camera = Camera3D::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
  //camera->SetOrtho(true);
  arcball = camera->CreateArcball();

  shadow_camera = Camera3D::Make(2.0f, 3.5f, 4.0f);

  //LightPtr light = ObjLight::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
  LightPtr light = Light::Make(2.0f, 3.5f, 4.0f,1.0f,"world");

  AppearancePtr white = Material::Make(1.0f,1.0f,1.0f);
  AppearancePtr yellow = Material::Make(1.0f, 1.0f, 0);
  AppearancePtr red = Material::Make(1.0f, 0.5f, 0.5f);
  AppearancePtr green = Material::Make(0.5f, 1.f, .5f);
  //AppearancePtr poff = PolygonOffset::Make(-10,-10);
  //AppearancePtr paper = Texture::Make("decal","../images/paper.jpg");

  AppearancePtr tex_earth = Texture::Make("decal", "./images/earth.jpg");
  AppearancePtr tex_white = Texture::Make("decal", glm::vec3(1.0f, 1.0f, 1.0f));

  AppearancePtr normal_earth = Texture::Make("normal", "./images/earth-normal.png");
  AppearancePtr normal_white = Texture::Make("normal", glm::vec3(0.5f, 0.5f, 1.0f));

  TexDepthPtr smap = TexDepth::Make("smap", DIM, DIM);
  smap->SetCompareMode();

  fbo = Framebuffer::Make(smap);

  // create sm shader
  shader_sm = Shader::Make(light, "camera");
  shader_sm->AttachVertexShader("./shaders/ilum_vert/vertex_sm.glsl");
  shader_sm->AttachFragmentShader("./shaders/ilum_vert/fragment_sm.glsl");
  shader_sm->Link();

  // Define a different shader for texture mapping
  // An alternative would be to use only this shader with a "white" texture for untextured objects
  shd_tex = Shader::Make(light, "camera");
  shd_tex->AttachVertexShader("./shaders/ilum_vert/vertex_texture.glsl");
  shd_tex->AttachFragmentShader("./shaders/ilum_vert/fragment_texture.glsl");
  shd_tex->Link();

  glViewport(0, 0, DIM, DIM);
  glm::mat4 bias(1.0f);
  bias = glm::translate(bias, glm::vec3(0.5f));
  bias = glm::scale(bias, glm::vec3(0.5f));
  glm::mat4 lightProj = shadow_camera->GetProjMatrix();
  glm::mat4 lightView = shadow_camera->GetViewMatrix();
  glm::mat4 mat = bias * lightProj * lightView;
  auto mtex = Variable<glm::mat4>::Make("Mtex", mat);
  int width, height;
  glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
  glViewport(0, 0, width, height);

  TransformPtr trf_table = Transform::Make();
  trf_table->Scale(3.0f,0.3f,3.0f);
  trf_table->Translate(0.0f,-1.0f,0.0f);

  TransformPtr trf_box = Transform::Make();
  trf_box->Scale(1.f,.5f,1.f);
  
  TransformPtr trf_ball = Transform::Make();
  trf_ball->Scale(.2f, .2f, .2f);
  trf_ball->Translate(-0.8f, 3.5f, -0.8f);

  TransformPtr trf_earth = Transform::Make();
  trf_earth->Scale(.5f, .5f, .5f);
  trf_earth->Translate(-2.f, 1.f, -2.f);

  Error::Check("before shps");
  Error::Check("before cube");
  ShapePtr cube = Cube::Make();
  //Error::Check("before quad");
  //ShapePtr quad = Quad::Make();
  Error::Check("before sphere");
  ShapePtr sphere = Sphere::Make();
  Error::Check("after shps");

  // build scene

  auto table = Node::Make(trf_table, {white,tex_white,normal_white}, {cube});
  auto box = Node::Make(trf_box, {yellow,tex_white,normal_white}, {cube});
  auto ball = Node::Make(trf_ball, {red,tex_white,normal_white}, {sphere});
  auto earth = Node::Make(trf_earth, {white,tex_earth,normal_earth}, {sphere});

  NodePtr root = Node::Make(shd_tex, {mtex, smap}, { table, box, ball, earth });
  scene = Scene::Make(root);
}

static void display (GLFWwindow* win)
{ 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window 

  fbo->Bind();
  glClear(GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, DIM, DIM);
  //glPolygonOffset(1.0f, 1.0f);
  glCullFace(GL_FRONT);
  glEnable(GL_POLYGON_OFFSET_FILL);
  Error::Check("before sm render");
  scene->GetRoot()->SetShader(shader_sm);
  scene->Render(shadow_camera);
  Error::Check("after sm render");
  glDisable(GL_POLYGON_OFFSET_FILL);
  glCullFace(GL_BACK);
  glFlush();
  fbo->Unbind();

  int height, width;
  glfwGetFramebufferSize(win, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Error::Check("before render");
  scene->GetRoot()->SetShader(shd_tex);
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

