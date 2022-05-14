#include "RenderToFile.h"
#include "external/stb_image/stb_image_write.h"
#include <memory>
#include <GL/glew.h>

namespace pcg
{
    void renderToFile(const std::string& file, uint32_t width, uint32_t height)
    {
        uint32_t channelNumber = 4;
        uint32_t bytes = width * height * channelNumber;
        auto data = std::make_unique<uint8_t[]>(bytes);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(
            0,
            0,
            width,
            height,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data.get());
        stbi_flip_vertically_on_write(true);
        stbi_write_png_compression_level = 0;
        stbi_write_png(
            file.c_str(),
            width,
            height,
            channelNumber,
            data.get(),
            width * channelNumber);
        stbi_flip_vertically_on_write(false);
    }
}
