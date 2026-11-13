/*
 * Icon mapping: the colour table that Urm__MapIconAllocate indexes with the
 * pixel values of an icon, and the pixel size that selects how many bits make
 * one pixel.  Run by run-mrm-icon-bounds.py with the production lib/Mrm
 * sources compiled under AddressSanitizer and UndefinedBehaviorSanitizer.
 *
 *   check-mrm-icon-bounds CASE TMPDIR
 *
 * Icons and colour tables are built in memory with exactly the size of their
 * contents, so a read behind them is reported by AddressSanitizer.  The display
 * and the screen are the stand-ins of tests/fuzz/mrm-stubs.c.
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>
#include <Mrm/Mrm.h>

#include "mrm-stubs.h"

/* The toolkit headers redefine assert(), so the checks use their own macro. */
#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #condition); \
            abort(); \
        } \
    } while (0)

/* A colour table of COUNT entries in a block of exactly that size. */
static RGMColorTablePtr new_table(int count)
{
    size_t size = offsetof(RGMColorTable, item) + (size_t) count * sizeof(RGMColorTableEntry);
    RGMColorTablePtr table = calloc(1, size);

    CHECK(table != NULL);
    table->validation = URMColorTableValid;
    table->count = (MrmCount) count;
    for (int i = 0; i < count; i++)
        table->item[i].color_pixel = (Pixel) (100 + i);
    return table;
}

/* An icon whose pixel data is a block of exactly SIZE bytes. */
static void new_icon(RGMIconImage *icon, int width, int height, int pixel_size,
                     const unsigned char *data, size_t size)
{
    char *copy = malloc(size);

    CHECK(copy != NULL);
    memcpy(copy, data, size);
    memset(icon, 0, sizeof *icon);
    icon->width = (MrmCount) width;
    icon->height = (MrmCount) height;
    icon->pixel_size = (MrmCount) pixel_size;
    icon->pixel_data.pdptr = copy;
}

/* The stand-in screen has a depth of 24, which Xlib stores in 32 bits per pixel;
 * the loader gets that number of bits from its caller as DSTPIX. */
static Cardinal map_icon(RGMIconImage *icon, int srcpix, RGMColorTablePtr table, Pixmap *pixmap)
{
    *pixmap = 0;
    return Urm__MapIconAllocate(icon, srcpix, 32, table, fuzz_screen(), fuzz_display(), pixmap, NULL);
}

static void release(RGMIconImage *icon, RGMColorTablePtr table)
{
    free(icon->pixel_data.pdptr);
    free(table);
}

/* A 4 bit icon with all sixteen colours renders. */
static void valid_four_bit_icon(const char *dir)
{
    static const unsigned char data[] = { 0x10, 0x32, 0xFE, 0xCD };
    RGMIconImage icon;
    RGMColorTablePtr table = new_table(16);
    Pixmap pixmap;

    (void) dir;
    new_icon(&icon, 4, 2, URMPixelSize4Bit, data, sizeof data);
    CHECK(map_icon(&icon, 4, table, &pixmap) == MrmSUCCESS);
    CHECK(pixmap != 0);
    release(&icon, table);
}

/* A 1 bit icon needs only the two predefined entries. */
static void valid_one_bit_icon(const char *dir)
{
    static const unsigned char data[] = { 0xA5 };
    RGMIconImage icon;
    RGMColorTablePtr table = new_table(URMColorTableUserMin);
    Pixmap pixmap;

    (void) dir;
    new_icon(&icon, 8, 1, URMPixelSize1Bit, data, sizeof data);
    CHECK(map_icon(&icon, 1, table, &pixmap) == MrmSUCCESS);
    CHECK(pixmap != 0);
    release(&icon, table);
}

/* An 8 bit icon uses as many entries as its table holds. */
static void valid_full_eight_bit_icon(const char *dir)
{
    unsigned char data[256];
    RGMIconImage icon;
    RGMColorTablePtr table = new_table(256);
    Pixmap pixmap;

    (void) dir;
    for (int i = 0; i < 256; i++)
        data[i] = (unsigned char) i;
    new_icon(&icon, 256, 1, URMPixelSize8Bit, data, sizeof data);
    CHECK(map_icon(&icon, 8, table, &pixmap) == MrmSUCCESS);
    CHECK(pixmap != 0);
    release(&icon, table);
}

/* A pixel value that the colour table does not hold is a malformed icon. */
static void pixel_beyond_colour_table(const char *dir)
{
    static const unsigned char data[] = { 0x01, 0xF0, 0x02, 0x03 };
    RGMIconImage icon;
    RGMColorTablePtr table = new_table(4);
    Pixmap pixmap;

    (void) dir;
    new_icon(&icon, 4, 1, URMPixelSize8Bit, data, 4);
    CHECK(map_icon(&icon, 8, table, &pixmap) == MrmFAILURE);
    CHECK(pixmap == 0);
    release(&icon, table);
}

/* The last pixel of the row is the one beyond the table. */
static void last_pixel_beyond_colour_table(const char *dir)
{
    static const unsigned char data[] = { 0x00, 0x01, 0x02, 0x04 };
    RGMIconImage icon;
    RGMColorTablePtr table = new_table(4);
    Pixmap pixmap;

    (void) dir;
    new_icon(&icon, 4, 1, URMPixelSize8Bit, data, sizeof data);
    CHECK(map_icon(&icon, 8, table, &pixmap) == MrmFAILURE);
    release(&icon, table);
}

/* A pixel size that is none of the four codes cannot be decoded; the call
 * must return instead of looping. */
static void unknown_pixel_size(const char *dir)
{
    static const unsigned char data[] = { 0x00, 0x00, 0x00, 0x00 };
    static const int sizes[] = { 0, 5, -1, 255 };
    RGMColorTablePtr table = new_table(16);

    (void) dir;
    alarm(10);
    for (size_t i = 0; i < sizeof sizes / sizeof *sizes; i++) {
        RGMIconImage icon;
        Pixmap pixmap;

        new_icon(&icon, 4, 2, sizes[i], data, sizeof data);
        CHECK(map_icon(&icon, 4, table, &pixmap) == MrmFAILURE);
        free(icon.pixel_data.pdptr);
    }
    alarm(0);
    free(table);
}

static void missing_colour_table(const char *dir)
{
    static const unsigned char data[] = { 0x00, 0x00, 0x00, 0x00 };
    RGMIconImage icon;
    Pixmap pixmap;

    (void) dir;
    new_icon(&icon, 4, 2, URMPixelSize4Bit, data, sizeof data);
    CHECK(map_icon(&icon, 4, NULL, &pixmap) == MrmFAILURE);
    free(icon.pixel_data.pdptr);
}

/* The predefined background and foreground entries must exist. */
static void colour_table_too_small(const char *dir)
{
    static const unsigned char data[] = { 0x00, 0x00, 0x00, 0x00 };
    static const int counts[] = { 0, 1, -1 };

    (void) dir;
    for (size_t i = 0; i < sizeof counts / sizeof *counts; i++) {
        RGMIconImage icon;
        /* the block of a table that is too small holds only what it declares */
        RGMColorTablePtr table = new_table(counts[i] > 0 ? counts[i] : 1);
        Pixmap pixmap;

        table->count = (MrmCount) counts[i];
        new_icon(&icon, 4, 2, URMPixelSize4Bit, data, sizeof data);
        CHECK(map_icon(&icon, 4, table, &pixmap) == MrmFAILURE);
        release(&icon, table);
    }
}

static const struct {
    const char *name;
    void (*run)(const char *dir);
} cases[] = {
    { "pixel-beyond-colour-table", pixel_beyond_colour_table },
    { "last-pixel-beyond-colour-table", last_pixel_beyond_colour_table },
    { "unknown-pixel-size", unknown_pixel_size },
    { "missing-colour-table", missing_colour_table },
    { "colour-table-too-small", colour_table_too_small },
    { "valid-four-bit-icon", valid_four_bit_icon },
    { "valid-one-bit-icon", valid_one_bit_icon },
    { "valid-full-eight-bit-icon", valid_full_eight_bit_icon },
};

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s CASE TMPDIR\n", argv[0]);
        return 2;
    }
    for (size_t i = 0; i < sizeof cases / sizeof *cases; i++)
        if (strcmp(argv[1], cases[i].name) == 0) {
            MrmInitialize();
            cases[i].run(argv[2]);
            printf("PASS %s\n", cases[i].name);
            return 0;
        }
    fprintf(stderr, "unknown case %s\n", argv[1]);
    return 2;
}
