/*
* Function for rendering the image currently on the window to a file.
*/

#ifndef PCG_RENDERTOFILE_H
#define PCG_RENDERTOFILE_H

#include <string>

namespace pcg
{
    void renderToFile(const std::string& file, uint32_t width, uint32_t height);
}

#endif
