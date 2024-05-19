#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

GLuint loadPNG(const char * filename){

    FILE* fp = ::fopen(filename, "r");
    if (fp == NULL){
        printf("%s could not be opened.\n", filename); getchar();
        return 0;
    }
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr  = png_create_info_struct(png_ptr);


    png_init_io(png_ptr, fp);

    unsigned int sig_read = 0;
    png_set_sig_bytes(png_ptr, sig_read);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);


    png_uint_32 width, height; int bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

    unsigned long row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    unsigned char* data = (unsigned char*) ::malloc(row_bytes * height);

    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    // Align bytes as OpenGL expects them.
    for (unsigned int i = 0; i < height; i++) {
        memcpy(data+(row_bytes * i), row_pointers[i], row_bytes);
    }

    // Clean up after the read, and free any memory allocated
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL /* png_infopp_NULL */);

    // Close the file
    ::fclose(fp);
    
    GLint internalFormat = 0;

    // Available PNG formats:
    switch (color_type) {
        case PNG_COLOR_TYPE_RGBA:
            internalFormat = GL_RGBA;
            break;

        case PNG_COLOR_TYPE_RGB:
            internalFormat = GL_RGB;
            break;
        case PNG_COLOR_TYPE_GRAY:
            internalFormat = GL_LUMINANCE;
            break;

        case PNG_COLOR_TYPE_GRAY_ALPHA:
            internalFormat = GL_LUMINANCE_ALPHA;
            break;
    }

    // Find a free texture index or "name".
    GLuint texture;
    glGenTextures(1, &texture);

    // Find the index to global state
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // uv are not in [0,1] for some models, they assume uvs are repeated.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D,                 // What (target)
                 0,                                 // Mip-map level
                 internalFormat,                    // Internal format
                 width,                             // Width
                 height,                            // Height
                 0,                                 // Border
                 internalFormat,                    // Format (how to use)
                 GL_UNSIGNED_BYTE,                  // Type   (how to intepret)
                 data);                             // Data

    printf(" done.\n");
    glGenerateMipmap(GL_TEXTURE_2D);
    ::free(data);

    return texture;
}

