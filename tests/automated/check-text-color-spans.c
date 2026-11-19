/* Native regression test for XmTextSetColorSpans and XmTextClearColorSpans. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/TextP.h>
#include <Xm/TextOutP.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { \
    if (!(c)) { \
        fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); \
        exit(1); \
    } \
} while (0)

static void drain(XtAppContext app, Display *dpy)
{
    unsigned n = 0;
    XSync(dpy, False);
    while (XtAppPending(app)) {
        CHECK(++n < 10000);
        XtAppProcessEvent(app, XtIMAll);
        XSync(dpy, False);
    }
}

static Pixel alloc_rgb(Display *dpy, Colormap cmap, unsigned short r, unsigned short g, unsigned short b)
{
    XColor c;
    memset(&c, 0, sizeof(c));
    c.red = r;
    c.green = g;
    c.blue = b;
    c.flags = DoRed | DoGreen | DoBlue;
    CHECK(XAllocColor(dpy, cmap, &c));
    return c.pixel;
}

static int region_has_dominant_channel(Display *dpy, Colormap cmap, XImage *img,
                                       int x0, int y0, int x1, int y1, char chan)
{
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > img->width) x1 = img->width;
    if (y1 > img->height) y1 = img->height;
    int hits = 0;
    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            XColor qc;
            qc.pixel = XGetPixel(img, x, y);
            XQueryColor(dpy, cmap, &qc);
            if (chan == 'r' && qc.red > 45000 && qc.green < 20000 && qc.blue < 20000)
                ++hits;
            else if (chan == 'g' && qc.green > 40000 && qc.red < 20000 && qc.blue < 20000)
                ++hits;
            else if (chan == 'b' && qc.blue > 45000 && qc.red < 20000 && qc.green < 20000)
                ++hits;
        }
    }
    return hits;
}

static void test_widget_mode(XtAppContext app, Display *dpy, Boolean core_font)
{
    Colormap cmap = DefaultColormap(dpy, DefaultScreen(dpy));
    Pixel bg = alloc_rgb(dpy, cmap, 65535, 65535, 65535);
    Pixel fg = alloc_rgb(dpy, cmap, 4000, 4000, 4000);
    Pixel red = alloc_rgb(dpy, cmap, 58000, 4000, 4000);
    Pixel green = alloc_rgb(dpy, cmap, 4000, 48000, 4000);
    Pixel blue = alloc_rgb(dpy, cmap, 4000, 4000, 58000);

    Widget shell = XtVaAppCreateShell("colorspans", "ColorSpans",
                                      topLevelShellWidgetClass, dpy,
                                      XmNoverrideRedirect, True,
                                      XmNwidth, 460,
                                      XmNheight, 160,
                                      NULL);
    Arg args[12];
    Cardinal n = 0;
    XmFontList fl = NULL;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); ++n;
    XtSetArg(args[n], XmNrows, 5); ++n;
    XtSetArg(args[n], XmNcolumns, 48); ++n;
    XtSetArg(args[n], XmNbackground, bg); ++n;
    XtSetArg(args[n], XmNforeground, fg); ++n;
    XtSetArg(args[n], XmNvalue, "int main(void) {\n  return 42; /* ok */\n}\n"); ++n;
    if (core_font) {
        XFontStruct *fs = XLoadQueryFont(dpy, "fixed");
        CHECK(fs != NULL);
        fl = XmFontListCreate(fs, XmSTRING_DEFAULT_CHARSET);
        XtSetArg(args[n], XmNfontList, fl); ++n;
    }
    Widget text = XmCreateText(shell, "text", args, n);
    XtManageChild(text);
    if (fl)
        XmFontListFree(fl);

    /* Null and non-XmText safety */
    XmTextSetColorSpans(NULL, NULL, 0);
    XmTextSetColorSpans(shell, NULL, 0);
    XmTextClearColorSpans(NULL);
    XmTextClearColorSpans(shell);

    XtRealizeWidget(shell);
    drain(app, dpy);

    /* Overlapping & unsorted spans: later span overrides earlier span; adjacent merge */
    XmTextColorSpan spans[] = {
        { 19, 25, red },    /* "return" */
        { -5, 3, blue },    /* clamped to [0, 3) "int" */
        { 1, 3, red },      /* overrides [1, 3) to red, leaving [0, 1) blue */
        { 3, 4, red },      /* merges with [1, 3) -> [1, 4) */
        { 30, 999, green }, /* clamped to [30, last_pos) comment + brace */
    };
    XmTextSetColorSpans(text, spans, XtNumber(spans));
    drain(app, dpy);

    XmTextWidget tw = (XmTextWidget)text;
    OutputData data = tw->text.output->data;
    CHECK(data->color_span_count == 4);
    CHECK(data->color_spans[0].left == 0 && data->color_spans[0].right == 1 && data->color_spans[0].foreground == blue);
    CHECK(data->color_spans[1].left == 1 && data->color_spans[1].right == 4 && data->color_spans[1].foreground == red);
    CHECK(data->color_spans[2].left == 19 && data->color_spans[2].right == 25 && data->color_spans[2].foreground == red);
    CHECK(data->color_spans[3].left == 30 && data->color_spans[3].foreground == green);

    /* Verify rendered pixels on screen */
    Position rx1 = 0, ry1 = 0, rx2 = 0, ry2 = 0;
    Position gx1 = 0, gy1 = 0, gx2 = 0, gy2 = 0;
    CHECK(XmTextPosToXY(text, 19, &rx1, &ry1));
    CHECK(XmTextPosToXY(text, 25, &rx2, &ry2));
    CHECK(XmTextPosToXY(text, 30, &gx1, &gy1));
    CHECK(XmTextPosToXY(text, 38, &gx2, &gy2));

    XImage *img = XGetImage(dpy, XtWindow(text), 0, 0, XtWidth(text), XtHeight(text), AllPlanes, ZPixmap);
    CHECK(img != NULL);
    int asc = (int)data->font_ascent + 2;
    int desc = (int)data->font_descent + 2;
    CHECK(region_has_dominant_channel(dpy, cmap, img, rx1, ry1 - asc, rx2, ry1 + desc, 'r') > 4);
    CHECK(region_has_dominant_channel(dpy, cmap, img, gx1, gy1 - asc, gx2, gy1 + desc, 'g') > 4);
    XDestroyImage(img);

    /* Primary selection and deselection must NOT wipe out color spans */
    XmTextSetSelection(text, 19, 25, CurrentTime);
    drain(app, dpy);
    XmTextClearSelection(text, CurrentTime);
    drain(app, dpy);
    CHECK(data->color_span_count == 4);
    img = XGetImage(dpy, XtWindow(text), 0, 0, XtWidth(text), XtHeight(text), AllPlanes, ZPixmap);
    CHECK(img != NULL);
    CHECK(region_has_dominant_channel(dpy, cmap, img, rx1, ry1 - asc, rx2, ry1 + desc, 'r') > 4);
    XDestroyImage(img);

    /* Insertion rebases downstream spans automatically */
    XmTextInsert(text, 0, "//\n");
    drain(app, dpy);
    CHECK(data->color_spans[2].left == 22 && data->color_spans[2].right == 28);

    /* Clear spans restores normal foreground */
    XmTextClearColorSpans(text);
    drain(app, dpy);
    CHECK(data->color_span_count == 0 && data->color_spans == NULL);
    CHECK(XmTextPosToXY(text, 22, &rx1, &ry1));
    CHECK(XmTextPosToXY(text, 28, &rx2, &ry2));
    img = XGetImage(dpy, XtWindow(text), 0, 0, XtWidth(text), XtHeight(text), AllPlanes, ZPixmap);
    CHECK(img != NULL);
    CHECK(region_has_dominant_channel(dpy, cmap, img, rx1, ry1 - asc, rx2, ry1 + desc, 'r') == 0);
    XDestroyImage(img);

    XtDestroyWidget(shell);
    drain(app, dpy);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Display *dpy;
    XtToolkitInitialize();
    app = XtCreateApplicationContext();
    dpy = XtOpenDisplay(app, NULL, "colorspans", "ColorSpans", NULL, 0, &argc, argv);
    CHECK(dpy != NULL);

    test_widget_mode(app, dpy, False);
    test_widget_mode(app, dpy, True);

    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    printf("PASS text color spans\n");
    return 0;
}
