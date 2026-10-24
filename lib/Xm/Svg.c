/* Bounded optional SVG rasterization for the existing Motif image path. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SvgI.h"

#include <cairo.h>
#include <librsvg/rsvg.h>
#include <X11/Xutil.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XM_SVG_MAX_SOURCE_BYTES (16U * 1024U * 1024U)
#define XM_SVG_MAX_EDGE 8192U

static int
read_svg_source(const char *filename, unsigned char **data_return,
                size_t *length_return)
{
    FILE *file;
    long file_length;
    size_t length;
    size_t read_length;
    unsigned char *data;

    *data_return = NULL;
    *length_return = 0;
    file = fopen(filename, "rb");
    if (file == NULL || fseek(file, 0, SEEK_END) != 0) {
        if (file != NULL)
            fclose(file);
        return 1;
    }
    file_length = ftell(file);
    if (file_length <= 0 || (unsigned long)file_length > XM_SVG_MAX_SOURCE_BYTES ||
        fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return 1;
    }
    length = (size_t)file_length;
    data = (unsigned char *)malloc(length);
    if (data == NULL) {
        fclose(file);
        return 1;
    }
    read_length = fread(data, 1, length, file);
    fclose(file);
    if (read_length != length) {
        free(data);
        return 1;
    }
    *data_return = data;
    *length_return = length;
    return 0;
}

static unsigned long
component_to_mask(unsigned char component, unsigned long mask)
{
    unsigned int shift = 0;
    unsigned int bits = 0;
    uint64_t max_value;
    uint64_t scaled;

    while (mask != 0 && (mask & 1UL) == 0) {
        ++shift;
        mask >>= 1;
    }
    while (mask != 0 && (mask & 1UL) != 0) {
        ++bits;
        mask >>= 1;
    }
    if (bits == 0 || bits >= 32)
        return 0;
    max_value = (UINT64_C(1) << bits) - 1;
    scaled = ((uint64_t)component * max_value + 127) / 255;
    return (unsigned long)(scaled << shift);
}

static XImage *
create_ximage(Screen *screen, unsigned int width, unsigned int height)
{
    Visual *visual = DefaultVisualOfScreen(screen);
    XImage *image;
    size_t byte_count;

    if ((visual->class != TrueColor && visual->class != DirectColor) ||
        visual->red_mask == 0 || visual->green_mask == 0 ||
        visual->blue_mask == 0)
        return NULL;
    image = XCreateImage(DisplayOfScreen(screen), visual,
                         DefaultDepthOfScreen(screen), ZPixmap, 0, NULL,
                         width, height, 32, 0);
    if (image == NULL || image->bytes_per_line <= 0 ||
        (size_t)image->bytes_per_line > SIZE_MAX / height) {
        if (image != NULL)
            XDestroyImage(image);
        return NULL;
    }
    byte_count = (size_t)image->bytes_per_line * height;
    image->data = (char *)malloc(byte_count);
    if (image->data == NULL) {
        XDestroyImage(image);
        return NULL;
    }
    return image;
}

int
_XmSvgGetImage(Screen *screen, const char *filename, Pixel background,
               XImage **image_return)
{
    unsigned char *source = NULL;
    size_t source_length = 0;
    RsvgHandle *handle = NULL;
    RsvgRectangle viewport;
    GError *error = NULL;
    double intrinsic_width;
    double intrinsic_height;
    unsigned int width;
    unsigned int height;
    cairo_surface_t *surface = NULL;
    cairo_t *cairo = NULL;
    XImage *image = NULL;
    XColor background_color;
    unsigned int x;
    unsigned int y;
    int status = 1;

    if (screen == NULL || filename == NULL || image_return == NULL)
        return 1;
    *image_return = NULL;
    if (read_svg_source(filename, &source, &source_length) != 0)
        return 1;
    handle = rsvg_handle_new_from_data(source, source_length, &error);
    free(source);
    source = NULL;
    if (handle == NULL)
        goto cleanup;
    if (!rsvg_handle_get_intrinsic_size_in_pixels(handle, &intrinsic_width,
                                                  &intrinsic_height) ||
        !isfinite(intrinsic_width) || !isfinite(intrinsic_height) ||
        intrinsic_width <= 0.0 || intrinsic_height <= 0.0 ||
        intrinsic_width > XM_SVG_MAX_EDGE || intrinsic_height > XM_SVG_MAX_EDGE)
        goto cleanup;
    width = (unsigned int)ceil(intrinsic_width);
    height = (unsigned int)ceil(intrinsic_height);
    if (width == 0 || height == 0)
        goto cleanup;

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
        goto cleanup;
    cairo = cairo_create(surface);
    if (cairo_status(cairo) != CAIRO_STATUS_SUCCESS)
        goto cleanup;
    background_color.pixel = background;
    if (!XQueryColor(DisplayOfScreen(screen), DefaultColormapOfScreen(screen),
                     &background_color))
        goto cleanup;
    cairo_set_source_rgb(cairo, background_color.red / 65535.0,
                         background_color.green / 65535.0,
                         background_color.blue / 65535.0);
    cairo_paint(cairo);
    viewport.x = 0.0;
    viewport.y = 0.0;
    viewport.width = width;
    viewport.height = height;
    if (!rsvg_handle_render_document(handle, cairo, &viewport, &error) ||
        cairo_status(cairo) != CAIRO_STATUS_SUCCESS)
        goto cleanup;
    cairo_surface_flush(surface);
    image = create_ximage(screen, width, height);
    if (image == NULL)
        goto cleanup;
    for (y = 0; y < height; ++y) {
        const unsigned char *row = cairo_image_surface_get_data(surface) +
                                   (size_t)y * cairo_image_surface_get_stride(surface);
        for (x = 0; x < width; ++x) {
            uint32_t argb;
            unsigned char red;
            unsigned char green;
            unsigned char blue;

            memcpy(&argb, row + (size_t)x * sizeof(argb), sizeof(argb));
            red = (unsigned char)((argb >> 16) & 0xffU);
            green = (unsigned char)((argb >> 8) & 0xffU);
            blue = (unsigned char)(argb & 0xffU);
            XPutPixel(image, x, y,
                      component_to_mask(red, image->red_mask) |
                      component_to_mask(green, image->green_mask) |
                      component_to_mask(blue, image->blue_mask));
        }
    }
    *image_return = image;
    image = NULL;
    status = 0;

cleanup:
    if (error != NULL)
        g_error_free(error);
    if (image != NULL)
        XDestroyImage(image);
    if (cairo != NULL)
        cairo_destroy(cairo);
    if (surface != NULL)
        cairo_surface_destroy(surface);
    if (handle != NULL)
        g_object_unref(handle);
    free(source);
    return status;
}
