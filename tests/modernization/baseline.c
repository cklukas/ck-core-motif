/* SPDX-License-Identifier: LGPL-2.0-or-later */
/*
 * WP-00 modernization baseline.
 *
 * This is deliberately a native installed-library consumer.  It records the
 * realized geometry, event activity, traversal/focus behavior and a small
 * actual-window image for a fixed representative hierarchy.  It is a baseline
 * oracle, not a performance target and not a replacement for visual review.
 */
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/Xm.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned long configure;
    unsigned long expose;
    unsigned long focus_in;
    unsigned long focus_out;
} EventCounts;

static void
count_events(Widget widget, XtPointer client_data, XEvent *event, Boolean *continue_dispatch)
{
    EventCounts *counts = (EventCounts *)client_data;
    (void)widget;
    (void)continue_dispatch;
    switch (event->type) {
    case ConfigureNotify:
        ++counts->configure;
        break;
    case Expose:
        if (event->xexpose.count == 0)
            ++counts->expose;
        break;
    case FocusIn:
        ++counts->focus_in;
        break;
    case FocusOut:
        ++counts->focus_out;
        break;
    default:
        break;
    }
}

static void
observe(Widget widget, EventCounts *counts)
{
    XtAddEventHandler(widget,
                      StructureNotifyMask | ExposureMask | FocusChangeMask,
                      False, count_events, counts);
}

static void
drain_events(XtAppContext app)
{
    while (XtAppPending(app)) {
        XEvent event;
        XtAppNextEvent(app, &event);
        XtDispatchEvent(&event);
    }
}

static int
write_window_ppm(Display *display, Window window, const char *path)
{
    XWindowAttributes attributes;
    XImage *image;
    FILE *file;
    int x, y;

    if (!XGetWindowAttributes(display, window, &attributes) ||
        attributes.width <= 0 || attributes.height <= 0)
        return 0;
    image = XGetImage(display, window, 0, 0,
                      (unsigned int)attributes.width,
                      (unsigned int)attributes.height,
                      AllPlanes, ZPixmap);
    if (!image)
        return 0;
    file = fopen(path, "wb");
    if (!file) {
        XDestroyImage(image);
        return 0;
    }
    if (fprintf(file, "P6\n%d %d\n255\n", attributes.width, attributes.height) < 0) {
        fclose(file);
        XDestroyImage(image);
        return 0;
    }
    for (y = 0; y < attributes.height; ++y) {
        for (x = 0; x < attributes.width; ++x) {
            unsigned long pixel = XGetPixel(image, x, y);
            unsigned char rgb[3];
            unsigned long red = pixel & image->red_mask;
            unsigned long green = pixel & image->green_mask;
            unsigned long blue = pixel & image->blue_mask;
            unsigned long red_max = image->red_mask;
            unsigned long green_max = image->green_mask;
            unsigned long blue_max = image->blue_mask;
            rgb[0] = (unsigned char)(red_max ? (red * 255UL) / red_max : 0);
            rgb[1] = (unsigned char)(green_max ? (green * 255UL) / green_max : 0);
            rgb[2] = (unsigned char)(blue_max ? (blue * 255UL) / blue_max : 0);
            if (fwrite(rgb, sizeof(rgb), 1, file) != 1) {
                fclose(file);
                XDestroyImage(image);
                return 0;
            }
        }
    }
    if (fclose(file) != 0) {
        XDestroyImage(image);
        return 0;
    }
    XDestroyImage(image);
    return 1;
}

static void
print_geometry(FILE *file, const char *name, Widget widget)
{
    Dimension width, height, border;
    Position x, y;
    Boolean realized;
    XtVaGetValues(widget,
                  XmNx, &x, XmNy, &y,
                  XmNwidth, &width, XmNheight, &height,
                  XmNborderWidth, &border,
                  NULL);
    realized = XtIsRealized(widget);
    fprintf(file, "GEOMETRY %s x=%d y=%d width=%u height=%u border=%u managed=%d realized=%d\n",
            name, (int)x, (int)y, (unsigned int)width, (unsigned int)height,
            (unsigned int)border, XtIsManaged(widget) ? 1 : 0, realized ? 1 : 0);
}

static int
focus_matches(Widget shell, Widget widget)
{
    return XtIsRealized(widget) && XmGetFocusWidget(shell) == widget;
}

static void
query_shell_size(Widget shell, unsigned int *width, unsigned int *height)
{
    Dimension current_width = 0;
    Dimension current_height = 0;

    XtVaGetValues(shell,
                  XmNwidth, &current_width,
                  XmNheight, &current_height,
                  NULL);
    *width = (unsigned int)current_width;
    *height = (unsigned int)current_height;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, form, title, text, button, toggle, scrolled, nested, row;
    EventCounts events = {0, 0, 0, 0};
    Display *display;
    FILE *output;
    const char *output_path = argc > 1 ? argv[1] : "modernization-baseline.txt";
    const char *image_path = argc > 2 ? argv[2] : "modernization-baseline.ppm";
    XrmDatabase database;
    XrmValue value;
    char *type = NULL;
    int failed = 0;
    unsigned int before_width, before_height;
    unsigned int after_width, after_height;

    shell = XtVaAppInitialize(&app, "CkMotifModernizationBaseline",
                              NULL, 0, &argc, argv,
                              NULL,
                              XmNwidth, 640, XmNheight, 480,
                              NULL);
    display = XtDisplay(shell);
    form = XmCreateForm(shell, "form", NULL, 0);
    title = XmCreateLabel(form, "title", NULL, 0);
    text = XmCreateTextField(form, "text", NULL, 0);
    button = XmCreatePushButton(form, "button", NULL, 0);
    toggle = XmCreateToggleButton(form, "toggle", NULL, 0);
    scrolled = XmCreateScrolledWindow(form, "scrolled", NULL, 0);
    nested = XmCreateForm(scrolled, "nested", NULL, 0);
    row = XmCreateRowColumn(nested, "row", NULL, 0);

    XtVaSetValues(title,
                  XmNtopAttachment, XmATTACH_FORM,
                  XmNleftAttachment, XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM,
                  XmNalignment, XmALIGNMENT_BEGINNING,
                  NULL);
    XtVaSetValues(text,
                  XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, title,
                  XmNleftAttachment, XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM,
                  NULL);
    XtVaSetValues(button,
                  XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, text,
                  XmNleftAttachment, XmATTACH_FORM,
                  NULL);
    XtVaSetValues(toggle,
                  XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, text,
                  XmNleftAttachment, XmATTACH_WIDGET,
                  XmNleftWidget, button,
                  NULL);
    XtVaSetValues(scrolled,
                  XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, button,
                  XmNleftAttachment, XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM,
                  XmNbottomAttachment, XmATTACH_FORM,
                  NULL);
    XtVaSetValues(nested, XmNwidth, 560, XmNheight, 240, NULL);
    XtVaSetValues(row, XmNorientation, XmVERTICAL, NULL);
    {
        XmString title_string = XmStringCreateLocalized("Modernization baseline");
        XmString button_string = XmStringCreateLocalized("Action");
        XmString toggle_string = XmStringCreateLocalized("Option");
        XtVaSetValues(title, XmNlabelString, title_string, NULL);
        XtVaSetValues(button, XmNlabelString, button_string, NULL);
        XtVaSetValues(toggle, XmNlabelString, toggle_string, NULL);
        XmStringFree(title_string);
        XmStringFree(button_string);
        XmStringFree(toggle_string);
    }

    XtManageChild(row);
    XmScrolledWindowSetAreas(scrolled, NULL, NULL, nested);
    XtManageChild(nested);
    XtManageChild(title);
    XtManageChild(text);
    XtManageChild(button);
    XtManageChild(toggle);
    XtManageChild(scrolled);
    XtManageChild(form);
    observe(shell, &events);
    observe(form, &events);
    observe(title, &events);
    observe(text, &events);
    observe(button, &events);
    observe(toggle, &events);
    observe(scrolled, &events);
    observe(nested, &events);
    observe(row, &events);
    XtRealizeWidget(shell);
    XSync(display, False);
    drain_events(app);
    query_shell_size(shell, &before_width, &before_height);

    if (!XmProcessTraversal(text, XmTRAVERSE_CURRENT)) {
        fprintf(stderr, "baseline focus traversal did not focus text field\n");
        failed = 1;
    } else {
        XSync(display, False);
        drain_events(app);
        if (!focus_matches(shell, text)) {
            fprintf(stderr, "baseline focus traversal did not focus text field\n");
            failed = 1;
        }
    }
    XtVaSetValues(shell, XmNwidth, 800, XmNheight, 600, NULL);
    XSync(display, False);
    drain_events(app);
    query_shell_size(shell, &after_width, &after_height);
    if (after_width != 800 || after_height != 600 ||
        after_width == before_width || after_height == before_height) {
        fprintf(stderr, "baseline shell resize failed: before=%ux%u after=%ux%u\n",
                before_width, before_height, after_width, after_height);
        failed = 1;
    }
    if (events.configure == 0 || events.expose == 0) {
        fprintf(stderr, "baseline did not observe configure/expose activity: %lu/%lu\n",
                events.configure, events.expose);
        failed = 1;
    }
    database = XrmGetDatabase(display);
    if (!database && XResourceManagerString(display))
        database = XrmGetStringDatabase(XResourceManagerString(display));
    if (database && XrmGetResource(database, "Xft.dpi", "Xft.Dpi", &type, &value))
        fprintf(stdout, "XFT_DPI %s\n", value.addr ? value.addr : "");
    else
        fprintf(stdout, "XFT_DPI unset\n");

    output = fopen(output_path, "w");
    if (!output) {
        fprintf(stderr, "cannot open %s: %s\n", output_path, strerror(errno));
        failed = 1;
    } else {
        fprintf(output, "SCHEMA 1\n");
        fprintf(output, "DISPLAY_VENDOR %s\n", ServerVendor(display));
        fprintf(output, "DISPLAY_RELEASE %d\n", VendorRelease(display));
        fprintf(output, "EVENTS configure=%lu expose=%lu focus_in=%lu focus_out=%lu\n",
                events.configure, events.expose, events.focus_in, events.focus_out);
        fprintf(output, "SHELL before=%ux%u after=%ux%u\n",
                before_width, before_height, after_width, after_height);
        print_geometry(output, "form", form);
        print_geometry(output, "title", title);
        print_geometry(output, "text", text);
        print_geometry(output, "button", button);
        print_geometry(output, "toggle", toggle);
        print_geometry(output, "scrolled", scrolled);
        print_geometry(output, "nested", nested);
        print_geometry(output, "row", row);
        if (fclose(output) != 0) {
            fprintf(stderr, "cannot close %s: %s\n", output_path, strerror(errno));
            failed = 1;
        }
    }
    if (!write_window_ppm(display, XtWindow(shell), image_path)) {
        fprintf(stderr, "cannot capture realized window to %s\n", image_path);
        failed = 1;
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    if (!failed)
        puts("PASS WP-00 modernization baseline");
    return failed;
}
