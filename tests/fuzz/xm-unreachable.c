/*
 * Drawing and text measuring entry points of Xlib.  The compound string,
 * render table and converter targets never draw: there is no server.  If a
 * harness reaches one of these, the harness is wrong, not the code under test.
 * No headers are included, so that the definitions do not have to match the
 * prototypes of Xlib.
 */
#include <stdio.h>
#include <stdlib.h>

#define UNREACHABLE(name) do { fprintf(stderr, "xm-unreachable: %s called\n", name); abort(); } while (0)

#define DRAWING(name) void name(void) { UNREACHABLE(#name); }
DRAWING(XDrawImageString) DRAWING(XDrawImageString16) DRAWING(XDrawLine) DRAWING(XDrawSegments)
DRAWING(XDrawString) DRAWING(XDrawString16) DRAWING(XTextExtents) DRAWING(XTextExtents16)
DRAWING(XTextWidth) DRAWING(XTextWidth16) DRAWING(XmbDrawImageString) DRAWING(XmbDrawString)
DRAWING(XmbTextEscapement) DRAWING(XmbTextExtents) DRAWING(XwcDrawImageString) DRAWING(XwcDrawString)
DRAWING(XwcTextEscapement) DRAWING(XwcTextExtents)

DRAWING(XFontsOfFontSet) DRAWING(XExtentsOfFontSet)
