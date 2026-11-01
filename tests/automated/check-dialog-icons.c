/* Native acceptance for the SVG/bitmap default-symbol selection contract. */
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/Display.h>
#include <Xm/Appearance.h>

/* A vector rendered at 2x must contain detail within source-pixel blocks.
 * Nearest-neighbor enlargement repeats each source pixel in a uniform 2x2. */
static int vector_detail(Display *display, Pixmap pixmap, unsigned width,
                         unsigned height)
{
    XImage *image = XGetImage(display, pixmap, 0, 0, width, height,
                              AllPlanes, ZPixmap);
    unsigned x, y, detailed = 0;
    if (!image) return 0;
    for (y = 0; y + 1 < height; y += 2)
        for (x = 0; x + 1 < width; x += 2) {
            unsigned long p = XGetPixel(image, x, y);
            if (p != XGetPixel(image, x + 1, y) ||
                p != XGetPixel(image, x, y + 1) ||
                p != XGetPixel(image, x + 1, y + 1)) ++detailed;
        }
    XDestroyImage(image);
    return detailed >= 8;
}

static int dimensions(Display *display, Pixmap pixmap, unsigned int *width,
                      unsigned int *height)
{
    Window root;
    int x, y;
    unsigned int border, depth;
    return pixmap != XmUNSPECIFIED_PIXMAP && pixmap != None &&
        XGetGeometry(display, pixmap, &root, &x, &y, width, height, &border, &depth);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "DialogIconTest", NULL, 0,
                                     &argc, argv, NULL, NULL);
    Display *display = XtDisplay(shell);
    Screen *screen = XtScreen(shell);
    if (!XmSetDisplayScaleOverride(display, 1.0)) return 12;
    const char *names[] = {"dialog_error", "dialog_information", "dialog_question",
                           "dialog_warning", "dialog_working"};
    unsigned char types[] = {XmDIALOG_ERROR, XmDIALOG_INFORMATION, XmDIALOG_QUESTION,
                              XmDIALOG_WARNING, XmDIALOG_WORKING};
    Boolean svg = getenv("XM_EXPECT_SVG_UNAVAILABLE") == NULL;
    unsigned int i, width = 0, height = 0;
    for (i = 0; i < XtNumber(names); ++i) {
        Widget box = XtVaCreateWidget("message", xmMessageBoxWidgetClass, shell,
                                      XmNdialogType, types[i], NULL);
        Pixmap symbol = XmUNSPECIFIED_PIXMAP;
        XtVaGetValues(box, XmNsymbolPixmap, &symbol, NULL);
        if (!dimensions(display, symbol, &width, &height) ||
            (svg ? width != 40 || height != 40 : width >= 40 || height >= 40)) {
            fprintf(stderr, "FAIL default symbol %s: %ux%u\n", names[i], width, height);
            return 1;
        }
        if (svg) {
            const Pixel backgrounds[] = {BlackPixelOfScreen(screen),
                                          WhitePixelOfScreen(screen)};
            for (unsigned n = 0; n < XtNumber(backgrounds); ++n) {
                XImage *surface;
                XtVaSetValues(box, XmNbackground, backgrounds[n], NULL);
                XtVaGetValues(box, XmNsymbolPixmap, &symbol, NULL);
                if (!dimensions(display, symbol, &width, &height)) return 17;
                surface = XGetImage(display, symbol, 0, 0, width, height,
                                    AllPlanes, ZPixmap);
                if (!surface || XGetPixel(surface, 0, 0) != backgrounds[n]) {
                    fprintf(stderr, "FAIL symbol background after change: %s\n", names[i]);
                    return 18;
                }
                XDestroyImage(surface);
            }
            const double live_scales[] = {1.5, 2.0, 1.0};
            for (unsigned n = 0; n < XtNumber(live_scales); ++n) {
                if (!XmSetDisplayScaleOverride(display, live_scales[n])) return 20;
                XtVaGetValues(box, XmNsymbolPixmap, &symbol, NULL);
                if (!dimensions(display, symbol, &width, &height) ||
                    width != (unsigned)(40 * live_scales[n]) || height != width) {
                    fprintf(stderr, "FAIL live symbol scale %s: %ux%u at %.1f\n",
                            names[i], width, height, live_scales[n]);
                    return 21;
                }
            }
            Pixmap first = XmGetPixmap(screen, (char *)names[i],
                BlackPixelOfScreen(screen), WhitePixelOfScreen(screen));
            Pixmap again = XmGetPixmap(screen, (char *)names[i],
                BlackPixelOfScreen(screen), WhitePixelOfScreen(screen));
            Pixmap scaled = XmGetScaledPixmap(box, (char *)names[i],
                BlackPixelOfScreen(screen), WhitePixelOfScreen(screen),
                DefaultDepthOfScreen(screen), 2.0);
            XImage *image;
            if (first != again || !dimensions(display, scaled, &width, &height) ||
                width != 80 || height != 80) return 2;
            if (!vector_detail(display, scaled, width, height)) {
                fprintf(stderr, "FAIL vector detail %s\n", names[i]);
                return 13;
            }
            image = XGetImage(display, first, 0, 0, 40, 40, AllPlanes, ZPixmap);
            if (!image || XGetPixel(image, 0, 0) != WhitePixelOfScreen(screen) ||
                XGetPixel(image, 20, 15) == WhitePixelOfScreen(screen)) return 3;
            XDestroyImage(image);
            {
                Pixmap dark = XmGetScaledPixmap(box, (char *)names[i],
                    WhitePixelOfScreen(screen), BlackPixelOfScreen(screen),
                    DefaultDepthOfScreen(screen), 2.0);
                if (dark == scaled || !dimensions(display, dark, &width, &height) ||
                    width != 80 || height != 80) return 15;
                image = XGetImage(display, dark, 0, 0, width, height, AllPlanes, ZPixmap);
                if (!image || XGetPixel(image, 0, 0) != BlackPixelOfScreen(screen))
                    return 16;
                XDestroyImage(image);
                XmDestroyPixmap(screen, dark);
            }
            XmDestroyPixmap(screen, first);
            XmDestroyPixmap(screen, again);
            XmDestroyPixmap(screen, scaled);
        }
        XtDestroyWidget(box);
    }
    {
        const char *extra[] = {"dialog_template", "dialog_file",
                               "dialog_selection", "dialog_prompt"};
        for (i = 0; i < XtNumber(extra); ++i) {
            Pixmap icon = XmGetScaledPixmap(shell, (char *)extra[i],
                BlackPixelOfScreen(screen), WhitePixelOfScreen(screen),
                DefaultDepthOfScreen(screen), 2.0);
            if (!svg) {
                if (icon != XmUNSPECIFIED_PIXMAP) return 6;
            } else {
                if (!dimensions(display, icon, &width, &height) ||
                    width != 80 || height != 80) return 7;
                if (!vector_detail(display, icon, width, height)) return 14;
                XmDestroyPixmap(screen, icon);
            }
        }
    }
    if (svg) {
        const double scales[] = {1.0, 1.5, 2.0, 3.0};
        Pixmap retained[4];
        for (i = 0; i < XtNumber(scales); ++i) {
            if (!XmSetDisplayScaleOverride(display, scales[i])) return 8;
            retained[i] = XmGetScaledPixmap(shell, "dialog_information",
                BlackPixelOfScreen(screen), WhitePixelOfScreen(screen),
                DefaultDepthOfScreen(screen), 0.0);
            if (!dimensions(display, retained[i], &width, &height) ||
                width != (unsigned int)(40 * scales[i]) || height != width)
                return 9;
            if (i && retained[i] == retained[i-1]) return 10;
        }
        for (i = 0; i < XtNumber(scales); ++i)
            XmDestroyPixmap(screen, retained[i]);
        if (!XmClearDisplayScaleOverride(display)) return 11;
    }
    {
        Pixmap custom = XCreatePixmap(display, RootWindowOfScreen(screen), 13, 17,
                                       DefaultDepthOfScreen(screen));
        Widget box = XtVaCreateWidget("custom", xmMessageBoxWidgetClass, shell,
            XmNdialogType, XmDIALOG_QUESTION, XmNsymbolPixmap, custom, NULL);
        Pixmap actual;
        XtVaGetValues(box, XmNsymbolPixmap, &actual, NULL);
        if (actual != custom) return 4;
        XtVaSetValues(box, XmNbackground, BlackPixelOfScreen(screen),
                      XmNforeground, WhitePixelOfScreen(screen), NULL);
        XtVaGetValues(box, XmNsymbolPixmap, &actual, NULL);
        if (actual != custom) return 19;
        if (!XmSetDisplayScaleOverride(display, 2.0)) return 22;
        XtVaGetValues(box, XmNsymbolPixmap, &actual, NULL);
        if (actual != custom) return 23;
        XtDestroyWidget(box);
        if (!XmSetDisplayScaleOverride(display, 1.0)) return 24;
        XFreePixmap(display, custom);
    }
    if (svg) {
        const double scales[] = {1.0, 1.5, 2.0, 1.0};
        XtVaSetValues(shell, XmNwidth, 200, XmNheight, 100, NULL);
        XtRealizeWidget(shell);
        for (unsigned dark = 0; dark < 2; ++dark) {
            Widget dialog, symbol_child;
            XmString text = XmStringCreateLocalized("A live dialog with scalable artwork");
            if (!XmApplyAppearanceProfile(display, dark ? XmAPPEARANCE_MODERN_DARK :
                                                          XmAPPEARANCE_MODERN_LIGHT)) return 25;
            dialog = XmCreateInformationDialog(shell, "liveDialog", NULL, 0);
            XtVaSetValues(dialog, XmNmessageString, text, NULL);
            XmStringFree(text);
            XtManageChild(dialog);
            symbol_child = XmMessageBoxGetChild(dialog, XmDIALOG_SYMBOL_LABEL);
            for (unsigned n = 0; n < XtNumber(scales); ++n) {
                Pixmap symbol; Dimension child_width, child_height;
                if (!XmSetDisplayScaleOverride(display, scales[n])) return 26;
                XSync(display, False);
                for (unsigned events = 0; XtAppPending(app); ++events) {
                    if (events > 10000) return 27;
                    XtAppProcessEvent(app, XtIMAll); XSync(display, False);
                }
                XtVaGetValues(dialog, XmNsymbolPixmap, &symbol, NULL);
                XtVaGetValues(symbol_child, XmNwidth, &child_width,
                              XmNheight, &child_height, NULL);
                if (!dimensions(display, symbol, &width, &height) ||
                    width != (unsigned)(40 * scales[n]) || height != width ||
                    child_width < width || child_height < height) return 28;
            }
            XtDestroyWidget(XtParent(dialog));
        }
    }
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    puts(svg ? "PASS SVG dialog icons: defaults, dimensions, cache, scaling and custom override"
             : "PASS bitmap dialog fallback: defaults, dimensions and custom override");
    return 0;
}
