/* Notebook's modern page and tab surfaces, including actual flap-edge pixels. */
#include <Xm/XmAll.h>
#include <Xm/NotebookP.h>
#include <Xm/GadgetP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static void flush(XtAppContext app, Display *dpy)
{
    XSync(dpy, False);
    while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
    XSync(dpy, False);
}

static int edges(Widget tab, Widget notebook, int vertical, int positive)
{
    XImage *image;
    Dimension width, height, shadow, highlight;
    Position x, y;
    Pixel surface, border;
    int open_x, open_y, outer_x, outer_y;
    XtVaGetValues(tab, XmNx, &x, XmNy, &y, XmNwidth, &width, XmNheight, &height,
        XmNbackground, &surface, XmNshadowThickness, &shadow,
        XmNhighlightThickness, &highlight, NULL);
    {
        XmAppearanceProfile profile;
        CHECK(XmGetAppearanceProfile(XtDisplay(notebook), &profile));
        if (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK)
            XtVaGetValues(notebook, XmNbackPageForeground, &border, NULL);
        else
            XtVaGetValues(tab, positive ? XmNbottomShadowColor : XmNtopShadowColor, &border, NULL);
    }
    if (XtIsWidget(tab)) x = y = 0;
    image = XGetImage(XtDisplay(notebook), XtWindow(tab), x, y, width, height,
        AllPlanes, ZPixmap);
    CHECK(image);
    if (vertical) {
        open_x = outer_x = width / 2;
        open_y = positive ? 0 : height - 1;
        outer_y = positive ? height - 1 : 0;
    } else {
        open_y = outer_y = height / 2;
        open_x = positive ? 0 : width - 1;
        outer_x = positive ? width - 1 : 0;
    }
    if (XGetPixel(image, open_x, open_y) != surface ||
        XGetPixel(image, outer_x, outer_y) != border)
        fprintf(stderr, "tab=%s vertical=%d positive=%d open=%lu expected=%lu outer=%lu expected=%lu\n",
            XtName(tab), vertical, positive, XGetPixel(image,open_x,open_y), surface,
            XGetPixel(image,outer_x,outer_y), border);
    for (unsigned inset = 0; inset < (unsigned)(shadow + highlight); ++inset) {
        int px = open_x, py = open_y;
        if (vertical) py += positive ? (int)inset : -(int)inset;
        else px += positive ? (int)inset : -(int)inset;
        CHECK(XGetPixel(image, px, py) == surface);
    }
    CHECK(XGetPixel(image, outer_x, outer_y) == border);
    XDestroyImage(image);
    return 0;
}

static int connection(Widget tab, Widget notebook, int vertical, int positive)
{
    Position x, y;
    Dimension width, height;
    Pixel frame;
    XImage *image;
    XtVaGetValues(tab, XmNx, &x, XmNy, &y, XmNwidth, &width, XmNheight, &height, NULL);
    XtVaGetValues(notebook, XmNframeBackground, &frame, NULL);
    if (vertical) { x += width / 2; y += positive ? -1 : height; }
    else { x += positive ? -1 : width; y += height / 2; }
    image = XGetImage(XtDisplay(notebook), XtWindow(notebook), x, y, 1, 1, AllPlanes, ZPixmap);
    CHECK(image);
    if (XGetPixel(image, 0, 0) != frame)
        fprintf(stderr, "closed page connection at %d,%d: pixel=%lu frame=%lu tab=%s vertical=%d positive=%d shadow=%u frameSize=%ux%u major=%u minor=%u\n", x,y,XGetPixel(image,0,0),frame, XtName(tab),vertical,positive, ((XmNotebookWidget)notebook)->notebook.shadow_thickness, ((XmNotebookWidget)notebook)->notebook.frame_width, ((XmNotebookWidget)notebook)->notebook.frame_height, ((XmNotebookWidget)notebook)->notebook.major_pos, ((XmNotebookWidget)notebook)->notebook.minor_pos);
    if (XGetPixel(image, 0, 0) != frame) {
        XmNotebookWidget nb = (XmNotebookWidget)notebook;
        fprintf(stderr,"nb border=%u binding=%u minorH=%u minorScrollH=%u backpage=%u pageH=%u tab border=%u\n", nb->core.border_width,nb->notebook.real_binding_width,nb->notebook.minor_height,nb->notebook.minor_scroller_height,nb->notebook.back_page_size,nb->notebook.page_height,tab->core.border_width);
        XImage *full = XGetImage(XtDisplay(notebook), XtWindow(notebook), 0, 0,
            XtWidth(notebook), XtHeight(notebook), AllPlanes, ZPixmap);
        FILE *file = fopen("notebook-failure.ppm", "wb");
        if (full && file) {
            fprintf(file, "P6\n%d %d\n255\n", full->width, full->height);
            for (int iy=0; iy<full->height; ++iy) for (int ix=0; ix<full->width; ++ix) {
                unsigned long pixel = XGetPixel(full,ix,iy);
                fputc((pixel >> 16) & 255, file); fputc((pixel >> 8) & 255, file); fputc(pixel & 255,file);
            }
        }
        if (file) fclose(file);
        if (full) XDestroyImage(full);
        fprintf(stderr, "selected major=%s minor=%s page=%d active tab geometry=%d,%d %ux%u\n",
            nb->notebook.top_major ? XtName(nb->notebook.top_major) : "none",
            nb->notebook.top_minor ? XtName(nb->notebook.top_minor) : "none",
            nb->notebook.current_page_number, XtX(tab), XtY(tab), XtWidth(tab), XtHeight(tab));
    }
    CHECK(XGetPixel(image, 0, 0) == frame);
    XDestroyImage(image);
    return 0;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Display *dpy;
    unsigned char placements[] = {XmBOTTOM_RIGHT, XmBOTTOM_LEFT, XmTOP_RIGHT, XmTOP_LEFT};
    XtToolkitInitialize();
    app = XtCreateApplicationContext();
    dpy = XtOpenDisplay(app, NULL, "notebookTest", "NotebookTest", NULL, 0, &argc, argv);
    CHECK(dpy);
    for (unsigned scale = 1; scale <= 2; ++scale)
    for (unsigned theme = 0; theme < 4; ++theme)
    for (unsigned minor = 0; minor < 2; ++minor)
    for (unsigned vertical = 0; vertical < 2; ++vertical)
    for (unsigned place = 0; place < XtNumber(placements); ++place) {
        Widget shell, notebook, tabs[2];
        Pixel surface, frame, back, tab_surface;
        Boolean edge_vertical = minor ? !vertical : vertical;
        Boolean positive = edge_vertical ? place < 2 : place % 2 == 0;
        CHECK(XmSetDisplayScaleOverride(dpy, scale));
        CHECK(XmSetAppearanceProfile(dpy, theme == 0 ? XmAPPEARANCE_CLASSIC : theme == 1 ? XmAPPEARANCE_MODERN_LIGHT : theme == 2 ? XmAPPEARANCE_MODERN_DARK : XmAPPEARANCE_HIGH_CONTRAST));
        shell = XtAppCreateShell("notebookTest", "NotebookTest", applicationShellWidgetClass, dpy, NULL, 0);
        notebook = XtVaCreateManagedWidget("notebook", xmNotebookWidgetClass, shell,
            XmNwidth, 440 * scale, XmNheight, 300 * scale,
            /* Keep content windows away from the pixels probing the page join. */
            XmNinnerMarginWidth, 8, XmNinnerMarginHeight, 8,
            XmNorientation, vertical ? XmVERTICAL : XmHORIZONTAL,
            XmNbackPagePlacement, placements[place], NULL);
        for (unsigned i = 0; i < 2; ++i) {
            XtVaCreateManagedWidget(i ? "pageTwo" : "pageOne", xmFormWidgetClass, notebook,
                XmNnotebookChildType, XmPAGE, XmNpageNumber, i+1, NULL);
            tabs[i] = XtVaCreateManagedWidget(i ? "gadgetTab" : "widgetTab",
                i ? xmPushButtonGadgetClass : xmPushButtonWidgetClass, notebook,
                XmNnotebookChildType, minor ? XmMINOR_TAB : XmMAJOR_TAB, XmNpageNumber, i+1, NULL);
        }
        if (scale == 2) XtVaSetValues(notebook, XmNframeShadowThickness, 0, NULL);
        XtRealizeWidget(shell);
        flush(app, dpy);
        XSetInputFocus(dpy, XtWindow(shell), RevertToParent, CurrentTime);
        flush(app, dpy);
        CHECK(XmProcessTraversal(tabs[0], XmTRAVERSE_CURRENT));
        flush(app, dpy);
        XtVaGetValues(notebook, XmNbackground, &surface, XmNframeBackground, &frame,
            XmNbackPageBackground, &back, NULL);
        CHECK(surface == frame);
        if (theme) CHECK(surface == back);
        for (unsigned i = 0; i < 2; ++i) {
            XtVaGetValues(tabs[i], XmNbackground, &tab_surface, NULL);
            if (theme) CHECK(surface == tab_surface);
        }
        CHECK(connection(tabs[0], notebook, edge_vertical, positive) == 0);
        CHECK(edges(tabs[1], notebook, edge_vertical, positive) == 0);
        XtVaSetValues(notebook, XmNcurrentPageNumber, 2, NULL);
        XSetInputFocus(dpy, XtWindow(shell), RevertToParent, CurrentTime);
        flush(app, dpy);
        CHECK(XmProcessTraversal(tabs[1], XmTRAVERSE_CURRENT));
        XClearArea(dpy, XtWindow(notebook), 0, 0, 0, 0, True);
        XClearArea(dpy, XtWindow(tabs[0]), 0, 0, 0, 0, True);
        flush(app, dpy);
        CHECK(XmGetFocusWidget(shell) == tabs[1]);
        CHECK(((XmGadget)tabs[1])->gadget.highlighted);
        CHECK(connection(tabs[1], notebook, edge_vertical, positive) == 0);
        CHECK(edges(tabs[0], notebook, edge_vertical, positive) == 0);
        CHECK(XmApplyAppearanceProfile(dpy, theme == 2 ? XmAPPEARANCE_MODERN_LIGHT : XmAPPEARANCE_MODERN_DARK));
        flush(app, dpy);
        XtVaGetValues(notebook, XmNbackground, &surface, XmNframeBackground, &frame, NULL);
        CHECK(surface == frame);
        CHECK(XmGetFocusWidget(shell) == tabs[1]);
        CHECK(((XmGadget)tabs[1])->gadget.highlighted);
        CHECK(connection(tabs[1], notebook, edge_vertical, positive) == 0);
        CHECK(edges(tabs[0], notebook, edge_vertical, positive) == 0);
        XtVaSetValues(notebook, XmNframeBackground, BlackPixel(dpy, DefaultScreen(dpy)),
            XmNinnerMarginWidth, 13, NULL);
        CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_CLASSIC));
        {
            Dimension margin = 0;
            XtVaGetValues(notebook, XmNframeBackground, &frame, XmNinnerMarginWidth, &margin, NULL);
            CHECK(frame == BlackPixel(dpy, DefaultScreen(dpy)) && margin == 13);
        }
        XtDestroyWidget(shell);
        flush(app, dpy);
    }
    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    puts("PASS Notebook connected page flaps");
    return 0;
}
