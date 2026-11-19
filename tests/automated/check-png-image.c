#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/PushBG.h>

static void extract_rgb(Display *dpy, Colormap cmap, unsigned long pixel,
                        int *r, int *g, int *b)
{
    XColor xc;
    xc.pixel = pixel;
    XQueryColor(dpy, cmap, &xc);
    *r = xc.red >> 8;
    *g = xc.green >> 8;
    *b = xc.blue >> 8;
}

static int near_rgb(int r, int g, int b, int er, int eg, int eb, int tol)
{
    return abs(r - er) <= tol && abs(g - eg) <= tol && abs(b - eb) <= tol;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Display *dpy;
    Screen *scr;
    Colormap cmap;
    XColor bg_xc, exact_xc;
    Pixmap pix_rgb, pix_rgba;
    XImage *img;
    int r = 0, g = 0, b = 0;

    if (argc < 3) {
        fprintf(stderr, "usage: %s <rgb.png> <rgba.png>\n", argv[0]);
        return 2;
    }

    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "CheckPngImage", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 64, XmNheight, 64, NULL);
    XtRealizeWidget(shell);

    dpy = XtDisplay(shell);
    scr = XtScreen(shell);
    cmap = DefaultColormapOfScreen(scr);

    if (!XAllocNamedColor(dpy, cmap, "#c4b2a8", &bg_xc, &exact_xc)) {
        fprintf(stderr, "FAIL: could not allocate background color\n");
        return 1;
    }

    /* 1. Verify 3-channel RGB PNG preserves red, green, and blue channels */
    pix_rgb = XmGetScaledPixmap(shell, argv[1], WhitePixelOfScreen(scr),
                                bg_xc.pixel, DefaultDepthOfScreen(scr), 1.0);
    if (pix_rgb == XmUNSPECIFIED_PIXMAP) {
        fprintf(stderr, "FAIL: XmGetScaledPixmap returned XmUNSPECIFIED_PIXMAP for RGB PNG\n");
        return 1;
    }
    img = XGetImage(dpy, pix_rgb, 0, 0, 4, 1, AllPlanes, ZPixmap);
    if (!img) {
        fprintf(stderr, "FAIL: XGetImage failed on RGB pixmap\n");
        return 1;
    }
    extract_rgb(dpy, cmap, XGetPixel(img, 0, 0), &r, &g, &b);
    if (!near_rgb(r, g, b, 255, 0, 0, 4)) {
        fprintf(stderr, "FAIL: RGB PNG pixel(0,0) expected red, got (%d,%d,%d)\n", r, g, b);
        return 1;
    }
    extract_rgb(dpy, cmap, XGetPixel(img, 1, 0), &r, &g, &b);
    if (!near_rgb(r, g, b, 0, 255, 0, 4)) {
        fprintf(stderr, "FAIL: RGB PNG pixel(1,0) expected green, got (%d,%d,%d)\n", r, g, b);
        return 1;
    }
    extract_rgb(dpy, cmap, XGetPixel(img, 2, 0), &r, &g, &b);
    if (!near_rgb(r, g, b, 0, 0, 255, 4)) {
        fprintf(stderr, "FAIL: RGB PNG pixel(2,0) expected blue, got (%d,%d,%d)\n", r, g, b);
        return 1;
    }
    XDestroyImage(img);
    XmDestroyPixmap(scr, pix_rgb);

    /* 2. Verify 4-channel RGBA PNG composites alpha=0 onto 16-bit XColor background */
    pix_rgba = XmGetScaledPixmap(shell, argv[2], WhitePixelOfScreen(scr),
                                 bg_xc.pixel, DefaultDepthOfScreen(scr), 1.0);
    if (pix_rgba == XmUNSPECIFIED_PIXMAP) {
        fprintf(stderr, "FAIL: XmGetScaledPixmap returned XmUNSPECIFIED_PIXMAP for RGBA PNG\n");
        return 1;
    }
    img = XGetImage(dpy, pix_rgba, 0, 0, 4, 1, AllPlanes, ZPixmap);
    if (!img) {
        fprintf(stderr, "FAIL: XGetImage failed on RGBA pixmap\n");
        return 1;
    }
    /* Pixel 0: opaque cyan (0, 255, 255, 255) */
    extract_rgb(dpy, cmap, XGetPixel(img, 0, 0), &r, &g, &b);
    if (!near_rgb(r, g, b, 0, 255, 255, 4)) {
        fprintf(stderr, "FAIL: RGBA PNG pixel(0,0) expected cyan, got (%d,%d,%d)\n", r, g, b);
        return 1;
    }
    /* Pixel 1: transparent (0, 0, 0, 0) -> must match #c4b2a8 (196, 178, 168) */
    extract_rgb(dpy, cmap, XGetPixel(img, 1, 0), &r, &g, &b);
    if (!near_rgb(r, g, b, 0xc4, 0xb2, 0xa8, 4)) {
        fprintf(stderr, "FAIL: RGBA PNG pixel(1,0) expected bg (196,178,168), got (%d,%d,%d)\n",
                r, g, b);
        return 1;
    }
    /* Pixel 2: opaque yellow (255, 255, 0, 255) */
    extract_rgb(dpy, cmap, XGetPixel(img, 2, 0), &r, &g, &b);
    if (!near_rgb(r, g, b, 255, 255, 0, 4)) {
        fprintf(stderr, "FAIL: RGBA PNG pixel(2,0) expected yellow, got (%d,%d,%d)\n", r, g, b);
        return 1;
    }
    XDestroyImage(img);

    /* 3. Verify an initially insensitive XmPushButtonGadget with a multi-color
     *    RGBA PNG renders on its very first Expose and preserves its colored
     *    background on transparent icon pixels. */
    {
        Widget rc, gad;
        Position gx = 0, gy = 0;
        Dimension gw = 0, gh = 0;
        int found_icon_gray = 0;
        int x, y;

        rc = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, shell,
                                     XmNbackground, bg_xc.pixel,
                                     XmNmarginWidth, 4,
                                     XmNmarginHeight, 4,
                                     NULL);
        gad = XtVaCreateManagedWidget("disabledIcon", xmPushButtonGadgetClass, rc,
                                      XmNbackground, bg_xc.pixel,
                                      XmNlabelType, XmPIXMAP,
                                      XmNlabelPixmap, pix_rgba,
                                      XmNmarginWidth, 2,
                                      XmNmarginHeight, 2,
                                      XmNshadowThickness, 0,
                                      XmNhighlightThickness, 0,
                                      NULL);
        XtSetSensitive(gad, False);

        XSync(dpy, False);
        while (XtAppPending(app)) {
            XEvent ev;
            XtAppNextEvent(app, &ev);
            XtDispatchEvent(&ev);
        }
        XSync(dpy, False);

        XtVaGetValues(gad, XmNx, &gx, XmNy, &gy, XmNwidth, &gw, XmNheight, &gh, NULL);
        if (gw < 4 || gh < 1) {
            fprintf(stderr, "FAIL: insensitive gadget got invalid size %ux%u\n",
                    (unsigned)gw, (unsigned)gh);
            return 1;
        }
        img = XGetImage(dpy, XtWindow(rc), gx, gy, gw, gh, AllPlanes, ZPixmap);
        if (!img) {
            fprintf(stderr, "FAIL: XGetImage failed on insensitive gadget area\n");
            return 1;
        }
        /* Top-left margin pixel (0,0) must keep exact #c4b2a8 tan background */
        extract_rgb(dpy, cmap, XGetPixel(img, 0, 0), &r, &g, &b);
        if (!near_rgb(r, g, b, 0xc4, 0xb2, 0xa8, 4)) {
            fprintf(stderr, "FAIL: insensitive gadget margin expected tan (196,178,168), got (%d,%d,%d)\n",
                    r, g, b);
            return 1;
        }
        for (y = 0; y < (int)gh; y++) {
            for (x = 0; x < (int)gw; x++) {
                extract_rgb(dpy, cmap, XGetPixel(img, x, y), &r, &g, &b);
                if (!near_rgb(r, g, b, 0xc4, 0xb2, 0xa8, 6)) {
                    /* Non-background icon pixel must be desaturated grayscale */
                    if (abs(r - g) > 4 || abs(g - b) > 4) {
                        fprintf(stderr, "FAIL: insensitive icon pixel (%d,%d) not grayscale: (%d,%d,%d)\n",
                                x, y, r, g, b);
                        return 1;
                    }
                    found_icon_gray = 1;
                }
            }
        }
        XDestroyImage(img);
        if (!found_icon_gray) {
            fprintf(stderr, "FAIL: insensitive XmPushButtonGadget did not paint icon on first Expose\n");
            return 1;
        }
    }

    XmDestroyPixmap(scr, pix_rgba);

    printf("PASS PNG RGB and RGBA image loading\n");
    return 0;
}
