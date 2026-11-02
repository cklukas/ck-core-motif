/* SPDX-License-Identifier: LGPL-2.0-or-later */
#ifndef DEMO_PAINT_H
#define DEMO_PAINT_H
#include <stdint.h>
#include <stddef.h>
#define PAINT_MAX_SIDE 4096
#define PAINT_HISTORY 16
#define PAINT_WHITE UINT32_C(0xffffff)
typedef struct { int width, height; uint32_t *pixels; } PaintImage;
typedef struct {
    PaintImage image, undo[PAINT_HISTORY], redo[PAINT_HISTORY];
    int undo_count, redo_count;
} PaintDocument;
int paint_image_create(PaintImage *, int, int);
void paint_image_free(PaintImage *);
int paint_image_copy(PaintImage *, const PaintImage *);
int paint_begin(PaintDocument *);
int paint_undo(PaintDocument *);
int paint_redo(PaintDocument *);
void paint_document_free(PaintDocument *);
uint32_t paint_get(const PaintImage *, int, int);
void paint_dot(PaintImage *, int, int, int, uint32_t);
void paint_line(PaintImage *, int, int, int, int, int, uint32_t);
void paint_rectangle(PaintImage *, int, int, int, int, int, int, uint32_t);
void paint_ellipse(PaintImage *, int, int, int, int, int, int, uint32_t);
int paint_fill(PaintImage *, int, int, uint32_t);
int paint_crop(PaintImage *, const PaintImage *, int, int, int, int);
void paint_paste(PaintImage *, const PaintImage *, int, int);
int paint_load(PaintImage *, const char *, char *, size_t);
int paint_save(const PaintImage *, const char *, char *, size_t);
#endif
