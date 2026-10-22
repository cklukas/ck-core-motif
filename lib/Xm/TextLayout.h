/* SPDX-License-Identifier: LGPL-2.0-or-later */
#ifndef _XmTextLayout_h
#define _XmTextLayout_h
#include <stddef.h>
#include <X11/Xft/Xft.h>
#include <X11/Xutil.h>
#ifdef __cplusplus
extern "C" {
#endif
/* An immutable line capture with shared paragraph analysis. Every coordinate,
 * visual hit, selection clip and painted glyph comes from this same capture. Font fallback
 * is selected for complete extended graphemes. All calls and destruction run
 * on the display's owning thread. The primary font must outlive the layout.
 * Text positions count Unicode scalars; only grapheme boundaries admit carets.
 * A tab is a direction-aware control advance; newlines belong to the host
 * paragraph/line composer. */
typedef struct _XmTextLayout XmTextLayout;
typedef struct _XmTextParagraph XmTextParagraph;
/* Paragraph text is immutable and includes complete embedding/isolate context.
 * Line ranges use scalar boundaries into this paragraph, never detached text.
 * Each line capture retains its paragraph until it is freed. */
XmTextParagraph *XmTextParagraphCreate(Display *, XftFont *, const char *utf8,
                                      size_t length, double tab_width);
XmTextParagraph *XmTextParagraphReference(XmTextParagraph *);
void XmTextParagraphFree(XmTextParagraph *);
size_t XmTextParagraphScalarCount(const XmTextParagraph *);
XmTextLayout *XmTextParagraphLine(XmTextParagraph *, size_t first, size_t last);
/* Greedy UAX #14 wrapping, with extended-grapheme emergency breaks only when
 * the first unbroken word exceeds the viewport. Captures are reshaped at their
 * actual line edges. A single overwide grapheme is retained to ensure progress.
 * last receives the next line's scalar offset in the same paragraph. */
XmTextLayout *XmTextParagraphWrapLine(XmTextParagraph *, size_t first,
                                      double width, size_t *last);
int XmTextLayoutAvailable(void);
XmTextLayout *XmTextLayoutCreate(Display *, XftFont *, const char *utf8, size_t length);
XmTextLayout *XmTextLayoutCreateWithTabWidth(Display *, XftFont *, const char *utf8,
                                           size_t length, double tab_width);
void XmTextLayoutFree(XmTextLayout *);
double XmTextLayoutWidth(const XmTextLayout *);
size_t XmTextLayoutScalarCount(const XmTextLayout *);
size_t XmTextLayoutGlyphCount(const XmTextLayout *);
int XmTextLayoutBoundary(const XmTextLayout *, size_t scalar, int direction, size_t *boundary);
double XmTextLayoutRangeWidth(const XmTextLayout *, size_t first, size_t last);
/* Affinity 0 is the logical leading edge of the following grapheme; 1 is
 * the logical trailing edge of the preceding grapheme. These are distinct
 * visual positions at a bidi boundary and coincide elsewhere. */
int XmTextLayoutCaret(const XmTextLayout *, size_t scalar, int affinity, double *x);
int XmTextLayoutHit(const XmTextLayout *, double x, size_t *scalar, int *affinity);
int XmTextLayoutVisualMove(const XmTextLayout *, size_t scalar, int affinity,
                           int direction, size_t *next_scalar, int *next_affinity);
/* Draw only the visual regions belonging to logical [first,last). The host's
 * viewport clip is intersected, including mixed-direction selections. A glyph
 * spanning a selection boundary is clipped from the same shaped capture. */
Region XmTextLayoutSelectionRegion(const XmTextLayout *, double x, size_t first, size_t last,
                                   const XRectangle *viewport);
int XmTextLayoutDraw(const XmTextLayout *, XftDraw *, const XftColor *,
                     double x, double baseline, size_t first, size_t last,
                     const XRectangle *viewport);
#ifdef __cplusplus
}
#endif
#endif
