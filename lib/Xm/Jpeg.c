#include "JpegI.h"
#include <jerror.h>
#include <stdlib.h>
#include <stdint.h>
#include <X11/Intrinsic.h>

void
_XmJpegErrorExit(j_common_ptr cinfo)
{
    int rc;
    XmJpegErrorMgr err = (XmJpegErrorMgr) cinfo->err;

    switch (cinfo->err->msg_code) {
    case JERR_NO_SOI:
        rc = 1;
        break;
    case JERR_OUT_OF_MEMORY:
        rc = 4;
        break;
    default:
        rc = 2;
        break;
    }
    longjmp(err->setjmp_buffer, rc);
}

int
load_jpeg(FILE * infile, unsigned long *pWidth, unsigned long *pHeight,
          CTable ** image_data)
{
    CTable *buf;
    struct jpeg_decompress_struct cinfo;
    XmJpegErrorMgrRec jerr;
    JSAMPROW row_pointer[1];
    int x, y;
    int rc;

    *image_data = NULL;
    cinfo.err = jpeg_std_error((struct jpeg_error_mgr *) &jerr);
    jerr.pub.error_exit = _XmJpegErrorExit;
    if ((rc = setjmp(jerr.setjmp_buffer))) {
        jpeg_destroy_decompress(&cinfo);
        return rc;
    }
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_calc_output_dimensions(&cinfo);
    jpeg_start_decompress(&cinfo);
    *pWidth = cinfo.output_width;
    *pHeight = cinfo.output_height;
    if (!cinfo.output_width || !cinfo.output_height ||
        (size_t)cinfo.output_width > SIZE_MAX / (size_t)cinfo.output_height ||
        (size_t)cinfo.output_width * (size_t)cinfo.output_height >
            SIZE_MAX / sizeof(CTable)) {
        jpeg_destroy_decompress(&cinfo);
        return 4;
    }
    *image_data = malloc((size_t)cinfo.output_width *
                         (size_t)cinfo.output_height * sizeof(CTable));
    if (!*image_data) {
        jpeg_destroy_decompress(&cinfo);
        return 4;
    }
    for (buf = *image_data;
         cinfo.output_scanline < cinfo.output_height;
         buf += cinfo.output_width)
        jpeg_read_scanlines(&cinfo, (JSAMPARRAY) (&buf), 1);
    if (cinfo.out_color_space == JCS_GRAYSCALE) {
        for (y = 0, buf = *image_data; y < cinfo.output_height;
             y++, buf += cinfo.output_width)
            for (x = cinfo.output_width - 1; x >= 0; x--)
                buf[x].red = buf[x].green = buf[x].blue =
                    ((JSAMPLE *) buf)[x];
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return 0;
}

Pixel
get_cval(unsigned char c, unsigned long mask)
{
    Pixel value = c, x;
    int i;
    for (i = 0, x = 1; i < 32; i++, x <<= 1)
        if (mask & x)
            break;
    for (; i < 32; i++, x <<= 1)
        if (!(mask & x))
            break;
    if (i < 8)
        value >>= 8 - i;
    else if (i > 8)
        value <<= i - 8;
    return (value & mask);
}

static int
store_pixel(Screen * screen, CTable * p, XImage *ximage, int x, int y)
{
    Pixel px = get_cval(p->red, screen->root_visual->red_mask)
        | get_cval(p->green, screen->root_visual->green_mask)
        | get_cval(p->blue, screen->root_visual->blue_mask);
    int pixel_bytes = (ximage->bits_per_pixel + 7) / 8;
    unsigned char *cdata;
    int byte;

    if (pixel_bytes < 1 || pixel_bytes > 4)
        return 0;
    cdata = (unsigned char *)ximage->data +
            (size_t)y * (size_t)ximage->bytes_per_line +
            (size_t)x * (size_t)pixel_bytes;
    if (ximage->byte_order == MSBFirst) {
        for (byte = 0; byte < pixel_bytes; byte++)
            cdata[byte] = (unsigned char)
                (px >> (8 * (pixel_bytes - byte - 1)));
    } else {
        for (byte = 0; byte < pixel_bytes; byte++)
            cdata[byte] = (unsigned char)(px >> (8 * byte));
    }
    return 1;
}

int
_XmJpegGetImage(Screen * screen, FILE * infile, XImage ** ximage)
{
    unsigned long image_width, image_height;
    unsigned char *xdata;
    size_t pixel_count, bytes_per_pixel;
    int pad;
    CTable *image_data = NULL;
    int rc;

    if ((rc = load_jpeg(infile, &image_width, &image_height, &image_data)))
        return rc;
    if (!image_width || !image_height || image_width > SIZE_MAX / image_height) {
        free(image_data);
        return 4;
    }
    pixel_count = (size_t)image_width * (size_t)image_height;
    if (screen->root_depth == 24 || screen->root_depth == 32) {
        bytes_per_pixel = 4;
        pad = 32;
    } else if (screen->root_depth == 16) {
        bytes_per_pixel = 2;
        pad = 16;
    } else {                    /* depth == 8 */
        bytes_per_pixel = 1;
        pad = 8;
    }
    if (pixel_count > SIZE_MAX / bytes_per_pixel) {
        free(image_data);
        return 4;
    }
    xdata = (unsigned char *) malloc(pixel_count * bytes_per_pixel);

    if (!xdata) {
      if (image_data)
        free(image_data);
      return 4;
    }

    *ximage =
        XCreateImage(screen->display, screen->root_visual,
                     screen->root_depth, ZPixmap, 0, (char *) xdata,
                     image_width, image_height, pad, 0);
    if (!*ximage) {
        free(xdata);
        if (image_data)
          free(image_data);
        return 4;
    }

    {
        int xx, yy;
        CTable *p;
        for (yy = 0; yy < (*ximage)->height; yy++) {
            p = image_data + yy * (*ximage)->width;
            for (xx = 0; xx < (*ximage)->width; xx++, p++)
                if (!store_pixel(screen, p, *ximage, xx, yy)) {
                    XDestroyImage(*ximage);
                    *ximage = NULL;
                    free(image_data);
                    return 2;
                }
        }
    }

    if (image_data)
        free(image_data);

    return 0;
}
