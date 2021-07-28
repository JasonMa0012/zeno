#include "stdafx.hpp"
#include "main.hpp"
#include "IGraphic.hpp"
#include <Hg/FPSCounter.hpp>
#include <sstream>
#include <cstdlib>
#include <array>
#include <stb_image_write.h>

namespace zenvis {

int curr_frameid = -1;

static bool playing = true;
static bool show_grid = true;

static int nx = 960, ny = 800;

static glm::vec3 bgcolor(0.23f, 0.23f, 0.23f);

static double last_xpos, last_ypos;
static glm::dvec3 center;

static glm::mat4x4 view(1), proj(1);
static float point_scale = 1.f;

void set_perspective(
    std::array<double, 16> viewArr,
    std::array<double, 16> projArr)
{
  std::memcpy(glm::value_ptr(view), viewArr.data(), viewArr.size());
  std::memcpy(glm::value_ptr(proj), projArr.data(), projArr.size());
}

void look_perspective(
    double cx, double cy, double cz,
    double theta, double phi, double radius,
    double fov, bool ortho_mode) {
  glm::dvec3 center(cx, cy, cz);

  point_scale = ny / (50.f * tanf(fov*0.5f*3.1415926f/180.0f));

  double cos_t = glm::cos(theta), sin_t = glm::sin(theta);
  double cos_p = glm::cos(phi), sin_p = glm::sin(phi);
  glm::dvec3 back(cos_t * sin_p, sin_t, -cos_t * cos_p);
  glm::dvec3 up(-sin_t * sin_p, cos_t, sin_t * cos_p);

  if (ortho_mode) {
    view = glm::lookAt(center - back, center, up);
    proj = glm::ortho(-radius * nx / ny, radius * nx / ny, -radius, radius,
                      -100.0, 100.0);
  } else {
    view = glm::lookAt(center - back * radius, center, up);
    proj = glm::perspective(glm::radians(fov), nx * 1.0 / ny, 0.05, 500.0);
  }
}

void set_program_uniforms(Program *pro) {
  pro->use();

  auto pers = proj * view;
  pro->set_uniform("mVP", pers);
  pro->set_uniform("mInvVP", glm::inverse(pers));
  pro->set_uniform("mView", view);
  pro->set_uniform("mProj", proj);
  pro->set_uniform("mInvView", glm::inverse(view));
  pro->set_uniform("mInvProj", glm::inverse(proj));
  pro->set_uniform("mPointScale", point_scale);
}

static std::unique_ptr<VAO> vao;
static std::unique_ptr<IGraphic> grid;
static std::unique_ptr<IGraphic> axis;

std::unique_ptr<IGraphic> makeGraphicGrid();
std::unique_ptr<IGraphic> makeGraphicAxis();

void initialize() {
  gladLoadGL();

  //CHECK_GL(glEnable(GL_BLEND));//??
  //CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  CHECK_GL(glEnable(GL_DEPTH_TEST));
  CHECK_GL(glEnable(GL_PROGRAM_POINT_SIZE));
  //CHECK_GL(glEnable(GL_POINT_SPRITE_ARB));
  //CHECK_GL(glEnable(GL_SAMPLE_COVERAGE));
  //CHECK_GL(glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE));
  //CHECK_GL(glEnable(GL_SAMPLE_ALPHA_TO_ONE));
  CHECK_GL(glEnable(GL_MULTISAMPLE));
  vao = std::make_unique<VAO>();
  grid = makeGraphicGrid();
  axis = makeGraphicAxis();
}

static void paint_graphics(void) {
  CHECK_GL(glViewport(0, 0, nx, ny));
  CHECK_GL(glClearColor(bgcolor.r, bgcolor.g, bgcolor.b, 0.0f));
  CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  vao->bind();
  if (show_grid) {
    grid->draw();
    axis->draw();
  }
  for (auto const &[key, gra]: current_frame_data()->graphics) {
    gra->draw();
  }
  vao->unbind();
  CHECK_GL(glFlush());
}

double get_time() {
  static auto start = std::chrono::system_clock::now();
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = now - start;
  return diff.count();
}

static hg::FPSCounter solverFPS(get_time, 1);
static hg::FPSCounter renderFPS(get_time, 10);

void finalize() {
  vao = nullptr;
}

void new_frame() {
  paint_graphics();
  renderFPS.tick();
}

void set_window_size(int nx_, int ny_) {
  nx = nx_;
  ny = ny_;
}

void set_curr_playing(bool playing_) {
  playing = playing_;
}

void set_curr_frameid(int frameid) {
  curr_frameid = std::max(frameid, 0);
}

int get_curr_frameid() {
  return curr_frameid;
}

double get_render_fps() {
  return renderFPS.fps();
}

double get_solver_interval() {
  return solverFPS.interval();
}

void set_show_grid(bool flag) {
    show_grid = flag;
}

void do_screenshot(std::string path) {
    CHECK_GL(glReadBuffer(GL_FRONT));
    void *pixels = malloc(nx * ny * 3);
    CHECK_GL(glReadPixels(0, 0, nx, ny, GL_RGB, GL_UNSIGNED_BYTE, pixels));
    stbi_flip_vertically_on_write(true);
    stbi_write_png(path.c_str(), nx, ny, 3, pixels, 0);
    free(pixels);
}

void new_frame_offline(std::string path) {
    GLuint fbo, rbo1, rbo2;
    CHECK_GL(glGenRenderbuffers(1, &rbo1));
    CHECK_GL(glGenRenderbuffers(1, &rbo2));
    CHECK_GL(glBindRenderbuffer(GL_RENDERBUFFER, rbo1));
    CHECK_GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, nx, ny));
    CHECK_GL(glBindRenderbuffer(GL_RENDERBUFFER, rbo2));
    CHECK_GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, nx, ny));

    CHECK_GL(glGenFramebuffers(1, &fbo));
    CHECK_GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo));
    CHECK_GL(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo1));
    CHECK_GL(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo2));
    CHECK_GL(glDrawBuffer(GL_COLOR_ATTACHMENT0));

    paint_graphics();

    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        CHECK_GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
        CHECK_GL(glBlitFramebuffer(0, 0, nx, ny, 0, 0, nx, ny, GL_COLOR_BUFFER_BIT, GL_NEAREST));
        CHECK_GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
        CHECK_GL(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
        CHECK_GL(glReadBuffer(GL_COLOR_ATTACHMENT0));

        char buf[1024];
        sprintf(buf, "%s/%06d.png", path.c_str(), curr_frameid);
        printf("saving screen %dx%d to %s\n", nx, ny, buf);

        void *pixels = malloc(nx * ny * 3);
        CHECK_GL(glReadPixels(0, 0, nx, ny, GL_RGB, GL_UNSIGNED_BYTE, pixels));
        stbi_flip_vertically_on_write(true);
        stbi_write_png(buf, nx, ny, 3, pixels, 0);
        free(pixels);
    }

    CHECK_GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    CHECK_GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    CHECK_GL(glDeleteRenderbuffers(1, &rbo1));
    CHECK_GL(glDeleteRenderbuffers(1, &rbo2));
    CHECK_GL(glDeleteFramebuffers(1, &fbo));
}

void set_background_color(float r, float g, float b) {
    bgcolor = glm::vec3(r, g, b);
}

std::tuple<float, float, float> get_background_color() {
    return {bgcolor.r, bgcolor.g, bgcolor.b};
}

}
