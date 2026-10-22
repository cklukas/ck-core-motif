/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/TextLayout.h>
#include <Xm/TextUnicode.h>
#include <X11/Xutil.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void require(int condition, const char *detail) { if (!condition) { fprintf(stderr, "%s\n", detail); exit(1); } }
static void check_positions(XmTextLayout *layout, const char *source) {
    XmTextUnicodeMap *map = XmTextUnicodeMapCreate(source, strlen(source));
    size_t g;
    require(map != NULL, "fixture UTF8 map");
    for (g = 0; g <= XmTextUnicodeGraphemeCount(map); ++g) {
        size_t scalar, hit;
        int affinity;
        double x, inverse;
        XmTextUnicodeScalarAtGrapheme(map, g, &scalar);
        require(XmTextLayoutCaret(layout, scalar, 0, &x), "missing grapheme caret");
        require(XmTextLayoutHit(layout, x, &hit, &affinity), "missing visual hit");
        require(XmTextUnicodeIsGraphemeBoundary(map, hit), "hit split extended grapheme");
        require(XmTextLayoutCaret(layout, hit, affinity, &inverse) && fabs(x - inverse) <= 0.01, "paint/caret/hit coordinate mismatch");
    }
    XmTextUnicodeMapFree(map);
}
int main(void) {
    Display *display = XOpenDisplay(NULL);
    XftFont *font;
    XmTextLayout *arabic, *mixed, *combining, *ligature, *fallback, *tabs;
    double beginning, end;
    const char *arabic_text = "\xd8\xa7\xd9\x84\xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a\xd8\xa9";
    const char *mixed_text = "abc \xd7\x90\xd7\x91\xd7\x92 \xd8\xa8\xd8\xad\xd8\xb1 xyz";
    XftDraw *draw;
    XftColor color;
    Pixmap pixmap;
    XRectangle viewport = {0, 0, 800, 120};
    XImage *image;
    unsigned long first;
    int changed = 0, x, y;
    require(display != NULL && XmTextLayoutAvailable(), "Unicode layout display/features unavailable");
    font = XftFontOpenName(display, DefaultScreen(display), "DejaVu Sans:size=18");
    require(font != NULL, "fixture font unavailable");
    arabic = XmTextLayoutCreate(display, font, arabic_text, strlen(arabic_text));
    mixed = XmTextLayoutCreate(display, font, mixed_text, strlen(mixed_text));
    combining = XmTextLayoutCreate(display, font, "e\xcc\x81", 3);
    ligature = XmTextLayoutCreate(display, font, "office", 6);
    fallback = XmTextLayoutCreate(display, font, "latin \xe4\xb8\xad\xe6\x96\x87", 12);
    tabs = XmTextLayoutCreateWithTabWidth(display, font, "a\tb", 3, 64);
    require(arabic != NULL, "Arabic paragraph creation failed");
    require(mixed != NULL, "mixed paragraph creation failed");
    require(combining != NULL, "combining paragraph creation failed");
    require(ligature != NULL, "ligature paragraph creation failed");
    require(fallback != NULL, "font fallback paragraph creation failed");
    require(tabs != NULL, "tab paragraph creation failed");
    check_positions(arabic, arabic_text); check_positions(mixed, mixed_text); check_positions(combining, "e\xcc\x81"); check_positions(ligature, "office");
    require(XmTextLayoutCaret(arabic, 0, 0, &beginning) && XmTextLayoutCaret(arabic, 7, 0, &end) && beginning > end, "RTL caret order is logical instead of visual");
    check_positions(fallback, "latin \xe4\xb8\xad\xe6\x96\x87"); check_positions(tabs, "a\tb");
    require(XmTextLayoutCaret(tabs, 2, 0, &beginning) && fabs(beginning - 64) < 0.01, "tab did not use the capture tab stop");
    require(!XmTextLayoutCaret(combining, 1, 0, &beginning), "combining interior admitted as caret");
    pixmap = XCreatePixmap(display, RootWindow(display, DefaultScreen(display)), 800, 120, DefaultDepth(display, DefaultScreen(display)));
    draw = XftDrawCreate(display, pixmap, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)));
    require(draw != NULL && XftColorAllocName(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), "white", &color), "fixture draw/color creation");
    XftDrawRect(draw, &color, 0, 0, 800, 120); XftColorFree(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &color);
    require(XftColorAllocName(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), "black", &color), "fixture foreground");
    require(XmTextLayoutDraw(mixed, draw, &color, 10, 50, 0, XmTextLayoutScalarCount(mixed), &viewport), "captured glyph draw failed");
    XSync(display, False); image = XGetImage(display, pixmap, 0, 0, 800, 120, AllPlanes, ZPixmap);
    require(image != NULL, "fixture readback failed");
    first = XGetPixel(image, 799, 119);
    for (y = 0; y < 120; ++y) for (x = 0; x < 800; ++x) if (XGetPixel(image, x, y) != first) ++changed;
    require(changed > 100, "shaped glyph capture painted no real text");
    XDestroyImage(image); XftColorFree(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &color);
    XftDrawDestroy(draw); XFreePixmap(display, pixmap);
    XmTextLayoutFree(arabic); XmTextLayoutFree(mixed); XmTextLayoutFree(combining); XmTextLayoutFree(ligature); XmTextLayoutFree(fallback); XmTextLayoutFree(tabs);
    XftFontClose(display, font); XCloseDisplay(display);
    puts("Shaped Arabic/Hebrew/Latin paragraph glyphs, grapheme carets and visual hits passed");
    return 0;
}
