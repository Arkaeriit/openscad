// Functions shared by OffscreenContext[platform].cc
// #include this directly after definition of struct OffscreenContext.

#include <vector>
#include <ostream>
#include "imageutils.h"

/*
   Capture framebuffer from OpenGL and write it to the given filename as PNG.
 */
bool save_framebuffer(const OffscreenContext *ctx, const char *filename)
{
  std::ofstream fstream(filename, std::ios::out | std::ios::binary);
  if (!fstream.is_open()) {
    std::cerr << "Can't open file " << filename << " for writing";
    return false;
  } else {
    save_framebuffer(ctx, fstream);
    fstream.close();
  }
  return true;
}

/*!
   Capture framebuffer from OpenGL and write it to the given ostream.
   Called by save_framebuffer() from platform-specific code.
 */
bool save_framebuffer_common(const OffscreenContext *ctx, std::ostream& output)
{
  if (!ctx) return false;
  const size_t samplesPerPixel = 4; // R, G, B and A
  std::vector<GLubyte> pixels(samplesPerPixel * ctx->width * ctx->height);
  glReadPixels(0, 0, ctx->width, ctx->height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

  // Flip it vertically - images read from OpenGL buffers are upside-down
  const size_t rowBytes = samplesPerPixel * ctx->width;
  auto *flippedBuffer = (unsigned char *)malloc(rowBytes * ctx->height);
  if (!flippedBuffer) {
    std::cerr << "Unable to allocate flipped buffer for corrected image.";
    return true;
  }
  flip_image(&pixels[0], flippedBuffer, samplesPerPixel, ctx->width, ctx->height);

  bool writeok = write_png(output, flippedBuffer, ctx->width, ctx->height);

  free(flippedBuffer);

  return writeok;
}

//	Called by create_offscreen_context() from platform-specific code.
OffscreenContext *create_offscreen_context_common(OffscreenContext *ctx)
{
  if (!ctx) return nullptr;
  GLenum err = glewInit(); // must come after Context creation and before FBO c$
  if (GLEW_OK != err) {
    std::cerr << "Unable to init GLEW: " << glewGetErrorString(err) << "\n";
    return nullptr;
  }

  return ctx;
}

