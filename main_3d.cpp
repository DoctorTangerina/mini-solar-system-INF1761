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
#include "texdepth.h"
#include "framebuffer.h"
#include "variable.h"

#include <iostream>
#include <cassert>

#define DIM 1024

static float viewer_pos[3] = {2.0f, 3.5f, 4.0f};

static float vec[DIM * DIM];

static ScenePtr scene, reflector, debug;
static Camera3DPtr camera, shadowcamera;
static ArcballPtr arcball;
static FramebufferPtr fbo;
static ShaderPtr shd_tex, shader_sm;

static void initialize (void)
{
  // set background color: white 
  glClearColor(1.0f,1.0f,1.0f,1.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);  // cull back faces

  // create objects
  camera = Camera3D::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
  //camera->SetOrtho(true);
  arcball = camera->CreateArcball();

  //LightPtr light = ObjLight::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
  LightPtr light = Light::Make(200.0f, 350.f, 400.0f, 1.0f, "world");

  shadowcamera = Camera3D::Make(200.0f, 350.f, 400.0f);
  shadowcamera->SetUpDir(1.0f, 0.0f, 0.0f);
  shadowcamera->SetAngle(90.f);
  shadowcamera->SetZPlanes(50.f, 500.f);

  auto near_plane = Variable<float>::Make("near_plane", 50.f);
  auto far_plane = Variable<float>::Make("far_plane", 500.f);

  //create shadow map
  TexDepthPtr smap = TexDepth::Make("smap", DIM, DIM);
  smap->SetCompareMode();
  //create framebuffer
  fbo = Framebuffer::Make(smap);

  AppearancePtr white = Material::Make(1.0f,1.0f,1.0f);
  AppearancePtr white_floor = Material::Make(1.0f, 1.0f, 1.0f, 0.5f);
  AppearancePtr yellow = Material::Make(1.0f, 1.0f, 0);
  AppearancePtr red = Material::Make(1.0f, 0.5f, 0.5f);
  AppearancePtr green = Material::Make(0.5f, 1.f, .5f);
  AppearancePtr poff = PolygonOffset::Make(-10,-10);
  //AppearancePtr paper = Texture::Make("decal","../images/paper.jpg");
  AppearancePtr tex_white = Texture::Make("decal", glm::vec3(1.0f));
  AppearancePtr tex_earth = Texture::Make("decal", "./images/earth.jpg");
  AppearancePtr normal_white = Texture::Make("normal", glm::vec3(0.5f, 0.5f, 1.0f));
  AppearancePtr normal_earth = Texture::Make("normal", "./images/earth-normal.png");
  //AppearancePtr tex_wood = Texture::Make("decal", "./images/wood.jpg");

  // create shader
  ShaderPtr shader = Shader::Make(light, "camera");
  shader->AttachVertexShader("./shaders/ilum_vert/vertex.glsl");
  shader->AttachFragmentShader("./shaders/ilum_vert/fragment.glsl");
  shader->Link();

  shader_sm = Shader::Make(light, "camera");
  shader_sm->AttachVertexShader("./shaders/ilum_vert/vertex_sm.glsl");
  shader_sm->AttachFragmentShader("./shaders/ilum_vert/fragment_sm.glsl");
  shader_sm->Link();

  ShaderPtr shader_debug = Shader::Make(light, "camera");
  shader_debug->AttachVertexShader("./shaders/ilum_vert/vertex_debug.glsl");
  shader_debug->AttachFragmentShader("./shaders/ilum_vert/fragment_debug.glsl");
  shader_debug->Link();

  // Define a different shader for texture mapping
  // An alternative would be to use only this shader with a "white" texture for untextured objects
  shd_tex = Shader::Make(light, "camera");
  shd_tex->AttachVertexShader("./shaders/ilum_vert/vertex_texture.glsl");
  shd_tex->AttachFragmentShader("./shaders/ilum_vert/fragment_texture.glsl");
  shd_tex->Link();

  TransformPtr trf_table = Transform::Make();
  trf_table->Scale(3.0f,0.3f,3.0f);
  trf_table->Translate(0.0f,-1.0f,0.0f);

  TransformPtr trf_box = Transform::Make();
  trf_box->Scale(1.f,.5f,1.f);
  
  TransformPtr trf_ball = Transform::Make();
  trf_ball->Scale(.2f, .2f, .2f);
  trf_ball->Translate(-0.8f, 3.5f, -0.8f);

  TransformPtr trf_can = Transform::Make();
  trf_can->Scale(.2f, .2f, .2f);
  trf_can->Translate(0.8f, 3.5f, 0.8f);

  TransformPtr trf_earth = Transform::Make();
  trf_earth->Scale(.5f, .5f, .5f);
  trf_earth->Translate(-2.f, 1.f, -2.f);

  TransformPtr trf_wood = Transform::Make();
  trf_wood->Scale(0.4f, 0.4f, 0.4f);
  trf_wood->Translate(2.5f, 1.f, 2.5f);

  TransformPtr trf_floor = Transform::Make();
  trf_floor->Translate(-1.5f, 0.0f, 1.5f);
  trf_floor->Rotate(90.f, -1, 0, 0);
  trf_floor->Scale(3.0f, 3.f, 1.0f);

  glm::mat4 translate = glm::translate(glm::mat4(1.0f),
      glm::vec3(0.5f, 0.5f, 0.5f));
  glm::mat4 scale = glm::scale(glm::mat4(1.0f),
      glm::vec3(0.5f, 0.5f, 0.5f));
  glm::mat4 mat = translate * scale *
      shadowcamera->GetProjMatrix() *
      shadowcamera->GetViewMatrix();
  auto mtex = Variable<glm::mat4>::Make("Mtex", mat);

  Error::Check("before shps");
  Error::Check("before cube");
  ShapePtr cube = Cube::Make();
  Error::Check("before quad");
  ShapePtr quad = Quad::Make(64, 64);
  Error::Check("before sphere");
  ShapePtr sphere = Sphere::Make();
  Error::Check("after shps");

  // build scene

  auto box = Node::Make(trf_box, {yellow, tex_white, normal_white}, {cube});
  auto ball = Node::Make(trf_ball, {red, tex_white, normal_white}, {sphere});
  auto earth = Node::Make(trf_earth, {white,tex_earth,normal_earth}, {sphere});
  auto floor = Node::Make(shd_tex, trf_floor, {smap, mtex, white_floor, tex_white, normal_white}, {quad});
  auto debug_quad = Node::Make(shader_debug, {smap, near_plane, far_plane}, { quad });

  NodePtr root = Node::Make(shd_tex, {smap, mtex}, { box, ball, earth });
  scene = Scene::Make(root);
  reflector = Scene::Make(floor);
  debug = Scene::Make(debug_quad);
}

static void display (GLFWwindow* win)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //desenha no stencil o refletor
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NEVER, 1, 0xFFFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    reflector->Render(camera);
    
    //desenha cena refletida
    glStencilFunc(GL_EQUAL, 1, 0xFFFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    NodePtr root = scene->GetRoot();
    TransformPtr trf = Transform::Make();
    trf->Scale(1.0f, -1.0f, 1.0f);
    root->SetTransform(trf);
    glFrontFace(GL_CW); //invert front face incidence
    scene->Render(camera);
    glFrontFace(GL_CCW); //restore front face incidence
    root->SetTransform(nullptr);
    glDisable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);

    //desenha sm
    Error::Check("before sm render");
    fbo->Bind();
    glClear(GL_DEPTH_BUFFER_BIT); // clear window
    glViewport(0, 0, DIM, DIM);
    glPolygonOffset(5.0f, 5.0f);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glDepthMask(GL_TRUE);
    scene->GetRoot()->SetShader(shader_sm);
    reflector->GetRoot()->SetShader(shader_sm);
    scene->Render(shadowcamera);
    reflector->Render(shadowcamera);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glFlush();
    fbo->Unbind();
    Error::Check("after sm render");

    {
        TexDepthPtr sm = fbo->GetDepthTexture();
        glBindTexture(GL_TEXTURE_2D, sm->GetTexId());
        glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, vec);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    for (float& i : vec) {
        if (i < 1.f) {
            std::cout << "temos depth: " << i << std::endl;
            throw Error();
        }
    }

    //desenha cena
    int width, height;
    glfwGetFramebufferSize(win, &width, &height);
    glViewport(0, 0, width, height);
    scene->Render(camera);
    glClear(GL_DEPTH_BUFFER_BIT); // clear window
    scene->GetRoot()->SetShader(shd_tex);
    scene->Render(camera);
    
    //debug
    //debug->Render(camera);
    
    //desenha refletor
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    reflector->GetRoot()->SetShader(shd_tex);
    reflector->Render(camera);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
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

