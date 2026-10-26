/* Native acceptance for the SVG/bitmap default-symbol selection contract. */
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/ImageCachI.h>

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
            image = XGetImage(display, first, 0, 0, 40, 40, AllPlanes, ZPixmap);
            if (!image || XGetPixel(image, 0, 0) != WhitePixelOfScreen(screen) ||
                XGetPixel(image, 20, 15) == WhitePixelOfScreen(screen)) return 3;
            XDestroyImage(image);
            XmDestroyPixmap(screen, first);
            XmDestroyPixmap(screen, again);
            XmDestroyPixmap(screen, scaled);
        }
        XtDestroyWidget(box);
    }
    {
        Pixmap custom = XCreatePixmap(display, RootWindowOfScreen(screen), 13, 17,
                                       DefaultDepthOfScreen(screen));
        Widget box = XtVaCreateWidget("custom", xmMessageBoxWidgetClass, shell,
            XmNdialogType, XmDIALOG_QUESTION, XmNsymbolPixmap, custom, NULL);
        Pixmap actual;
        XtVaGetValues(box, XmNsymbolPixmap, &actual, NULL);
        if (actual != custom) return 4;
        XtDestroyWidget(box);
        XFreePixmap(display, custom);
    }
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    puts(svg ? "PASS SVG dialog icons: defaults, dimensions, cache, scaling and custom override"
             : "PASS bitmap dialog fallback: defaults, dimensions and custom override");
    return 0;
}
