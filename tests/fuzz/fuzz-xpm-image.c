/*
 * Fuzz target: the XPM image and metadata parser and serializer (lib/Xm/Xpm*.c).
 *
 * The real lib/Xm/Xpm*.c sources (Xpmparse.c, Xpmdata.c, Xpmhashtab.c,
 * XpmCrIFrBuf.c, XpmCrBufFrI.c, XpmCrDatFrI.c, XpmCrIFrDat.c, XpmImage.c,
 * XpmInfo.c, XpmAttrib.c, Xpmmisc.c, Xpmrgb.c, Xpmscan.c, Xpmcreate.c) are
 * compiled unchanged with coverage and sanitizer instrumentation.
 *
 * The input is handed to XpmCreateXpmImageFromBuffer in a heap block of
 * exactly size + 1 bytes (NUL-terminated), so any read past the terminating
 * NUL is an immediate AddressSanitizer error.  When parsing succeeds, the
 * harness verifies the colour-table and pixel-index invariants and exercises
 * round-trip serialization through XpmCreateBufferFromXpmImage and
 * XpmCreateDataFromXpmImage.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Xm/XpmP.h>

static void verify_xpm_image(const XpmImage *image, const XpmInfo *info)
{
    if (image->width == 0 || image->height == 0 || image->ncolors == 0 || image->cpp == 0)
        abort();
    for (unsigned int c = 0; c < image->ncolors; c++) {
        const XpmColor *col = &image->colorTable[c];
        if (!col->string || strlen(col->string) != image->cpp)
            abort();
        if (col->symbolic) (void) strlen(col->symbolic);
        if (col->m_color) (void) strlen(col->m_color);
        if (col->g4_color) (void) strlen(col->g4_color);
        if (col->g_color) (void) strlen(col->g_color);
        if (col->c_color) (void) strlen(col->c_color);
    }
    size_t pixels = (size_t) image->width * (size_t) image->height;
    for (size_t i = 0; i < pixels; i++) {
        if (image->data[i] >= image->ncolors)
            abort();
    }
    if (info) {
        if ((info->valuemask & XpmComments)) {
            if (info->hints_cmt) (void) strlen(info->hints_cmt);
            if (info->colors_cmt) (void) strlen(info->colors_cmt);
            if (info->pixels_cmt) (void) strlen(info->pixels_cmt);
        }
        if ((info->valuemask & XpmExtensions) && info->extensions) {
            for (unsigned int e = 0; e < info->nextensions; e++) {
                if (info->extensions[e].name)
                    (void) strlen(info->extensions[e].name);
                for (unsigned int l = 0; l < info->extensions[e].nlines; l++) {
                    if (info->extensions[e].lines[l])
                        (void) strlen(info->extensions[e].lines[l]);
                }
            }
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    char *buf = malloc(size + 1);
    if (!buf) abort();
    if (size) memcpy(buf, data, size);
    buf[size] = '\0';

    XpmImage image;
    XpmInfo info;
    memset(&image, 0, sizeof(image));
    memset(&info, 0, sizeof(info));
    info.valuemask = XpmReturnComments | XpmReturnExtensions;

    int status = XpmCreateXpmImageFromBuffer(buf, &image, &info);
    if (status == XpmSuccess) {
        verify_xpm_image(&image, &info);

        size_t pixels = (size_t) image.width * (size_t) image.height;
        if (pixels <= 16384 && image.ncolors <= 512 && image.cpp <= 8) {
            char *out_buf = NULL;
            if (XpmCreateBufferFromXpmImage(&out_buf, &image, &info) == XpmSuccess && out_buf) {
                XpmImage rt_image;
                XpmInfo rt_info;
                memset(&rt_image, 0, sizeof(rt_image));
                memset(&rt_info, 0, sizeof(rt_info));
                rt_info.valuemask = XpmReturnComments | XpmReturnExtensions;
                if (XpmCreateXpmImageFromBuffer(out_buf, &rt_image, &rt_info) == XpmSuccess) {
                    verify_xpm_image(&rt_image, &rt_info);
                    XpmFreeXpmImage(&rt_image);
                }
                XpmFreeXpmInfo(&rt_info);
                XpmFree(out_buf);
            }

            char **out_data = NULL;
            if (XpmCreateDataFromXpmImage(&out_data, &image, &info) == XpmSuccess && out_data) {
                XpmImage dt_image;
                XpmInfo dt_info;
                memset(&dt_image, 0, sizeof(dt_image));
                memset(&dt_info, 0, sizeof(dt_info));
                if (XpmCreateXpmImageFromData(out_data, &dt_image, &dt_info) == XpmSuccess) {
                    verify_xpm_image(&dt_image, &dt_info);
                    XpmFreeXpmImage(&dt_image);
                }
                XpmFreeXpmInfo(&dt_info);
                XpmFree(out_data);
            }
        }
        XpmFreeXpmImage(&image);
    }
    XpmFreeXpmInfo(&info);
    free(buf);
    return 0;
}
