#include <Xm/XmAll.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "ScaledBitmap", NULL, 0, &argc, argv, NULL, NULL);
    Display *d = XtDisplay(shell); int screen = DefaultScreen(d);
    XImage *bits = XCreateImage(d, DefaultVisual(d, screen), 1, XYBitmap,
                               0, calloc(4, 1), 4, 4, 8, 0);
    assert(bits); XPutPixel(bits, 3, 3, 1);
    assert(XmInstallImage(bits, "scaled-bitmap-depth-test"));
    XColor fg, bg, exact;
    assert(XAllocNamedColor(d, DefaultColormap(d, screen), "red", &fg, &exact));
    assert(XAllocNamedColor(d, DefaultColormap(d, screen), "blue", &bg, &exact));
    const double scales[] = {1, 1.5, 2, 3};
    for (unsigned i = 0; i < XtNumber(scales); ++i) {
        Pixmap p = XmGetScaledPixmap(shell, "scaled-bitmap-depth-test", fg.pixel, bg.pixel,
                                     DefaultDepth(d, screen), scales[i]);
        assert(p != None && p != XmUNSPECIFIED_PIXMAP);
        unsigned size = (unsigned)(4 * scales[i]);
        XImage *pixels = XGetImage(d, p, 0, 0, size, size, AllPlanes, ZPixmap);
        assert(pixels);
        assert(XGetPixel(pixels, 0, 0) == bg.pixel);
        assert(XGetPixel(pixels, size - 1, size - 1) == fg.pixel);
        XDestroyImage(pixels); XmDestroyPixmap(XtScreen(shell), p);
    }
    assert(XmUninstallImage(bits)); XDestroyImage(bits);
    XtDestroyWidget(shell); puts("PASS Scaled bitmap depth"); return 0;
}
