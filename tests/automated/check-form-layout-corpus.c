/* SPDX-License-Identifier: LGPL-2.0-or-later */
/*
 * WP-10 installed-consumer diagnostic corpus.  This deliberately observes
 * public X events and interposes the Xt/Xlib entry points used for geometry
 * negotiation and redraw.  The counters are comparative diagnostics, not a
 * promise that they expose every private Form traversal.
 */
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/TextF.h>
#include <Xm/Xm.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
    ++checks; \
} while (0)

static unsigned long checks;
static unsigned long geometry_requests;
static unsigned long geometry_queries;
static unsigned long clear_area_calls;
static unsigned long copy_area_calls;
static unsigned long fill_rectangle_calls;

static XtGeometryResult (*real_geometry_request)(Widget, XtWidgetGeometry *,
                                                  XtWidgetGeometry *);
static XtGeometryResult (*real_query_geometry)(Widget, XtWidgetGeometry *,
                                                XtWidgetGeometry *);
static int (*real_clear_area)(Display *, Window, int, int, unsigned int,
                              unsigned int, Bool);
static int (*real_copy_area)(Display *, Drawable, Drawable, GC, int, int,
                             unsigned int, unsigned int, int, int);
static int (*real_fill_rectangle)(Display *, Drawable, GC, int, int,
                                  unsigned int, unsigned int);

XtGeometryResult
XtMakeGeometryRequest(Widget widget, XtWidgetGeometry *request,
                      XtWidgetGeometry *reply)
{
    if (!real_geometry_request)
        real_geometry_request = (XtGeometryResult (*)(Widget, XtWidgetGeometry *,
                                                       XtWidgetGeometry *))
            dlsym(RTLD_NEXT, "XtMakeGeometryRequest");
    if (!real_geometry_request)
        abort();
    ++geometry_requests;
    return real_geometry_request(widget, request, reply);
}

XtGeometryResult
XtQueryGeometry(Widget widget, XtWidgetGeometry *intended,
                XtWidgetGeometry *preferred)
{
    if (!real_query_geometry)
        real_query_geometry = (XtGeometryResult (*)(Widget, XtWidgetGeometry *,
                                                     XtWidgetGeometry *))
            dlsym(RTLD_NEXT, "XtQueryGeometry");
    if (!real_query_geometry)
        abort();
    ++geometry_queries;
    return real_query_geometry(widget, intended, preferred);
}

int
XClearArea(Display *display, Window window, int x, int y, unsigned int width,
           unsigned int height, Bool exposures)
{
    if (!real_clear_area)
        real_clear_area = (int (*)(Display *, Window, int, int, unsigned int,
                                   unsigned int, Bool))dlsym(RTLD_NEXT,
                                                             "XClearArea");
    if (!real_clear_area)
        abort();
    ++clear_area_calls;
    return real_clear_area(display, window, x, y, width, height, exposures);
}

int
XCopyArea(Display *display, Drawable source, Drawable destination, GC gc,
          int source_x, int source_y, unsigned int width, unsigned int height,
          int destination_x, int destination_y)
{
    if (!real_copy_area)
        real_copy_area = (int (*)(Display *, Drawable, Drawable, GC, int, int,
                                  unsigned int, unsigned int, int, int))
            dlsym(RTLD_NEXT, "XCopyArea");
    if (!real_copy_area)
        abort();
    ++copy_area_calls;
    return real_copy_area(display, source, destination, gc, source_x, source_y,
                          width, height, destination_x, destination_y);
}

int
XFillRectangle(Display *display, Drawable drawable, GC gc, int x, int y,
               unsigned int width, unsigned int height)
{
    if (!real_fill_rectangle)
        real_fill_rectangle = (int (*)(Display *, Drawable, GC, int, int,
                                       unsigned int, unsigned int))
            dlsym(RTLD_NEXT, "XFillRectangle");
    if (!real_fill_rectangle)
        abort();
    ++fill_rectangle_calls;
    return real_fill_rectangle(display, drawable, gc, x, y, width, height);
}

typedef struct {
    unsigned long configure;
    unsigned long expose;
    unsigned long map;
    unsigned long unmap;
} Events;

static void
observe_events(Widget widget, XtPointer client_data, XEvent *event,
               Boolean *continue_dispatch)
{
    Events *events = (Events *)client_data;
    (void)widget;
    (void)continue_dispatch;
    switch (event->type) {
    case ConfigureNotify:
        ++events->configure;
        break;
    case Expose:
        if (event->xexpose.count == 0)
            ++events->expose;
        break;
    case MapNotify:
        ++events->map;
        break;
    case UnmapNotify:
        ++events->unmap;
        break;
    default:
        break;
    }
}

static void
watch(Widget widget, Events *events)
{
    XtAddEventHandler(widget, StructureNotifyMask | ExposureMask, False,
                      observe_events, events);
}

static void
drain(XtAppContext app)
{
    while (XtAppPending(app)) {
        XEvent event;
        XtAppNextEvent(app, &event);
        XtDispatchEvent(&event);
    }
}

typedef struct {
    Widget form;
    Widget dynamic;
    Boolean managed;
    unsigned long activations;
} Mutation;

static void
mutate_layout(Widget widget, XtPointer client_data, XtPointer callback_data)
{
    Mutation *mutation = (Mutation *)client_data;
    (void)widget;
    (void)callback_data;
    mutation->managed = mutation->managed ? False : True;
    if (mutation->managed)
        XtManageChild(mutation->dynamic);
    else
        XtUnmanageChild(mutation->dynamic);
    XtVaSetValues(mutation->form, XmNmarginWidth,
                  mutation->managed ? 9 : 3, NULL);
    ++mutation->activations;
}

static int
valid_geometry(Widget widget)
{
    Dimension width = 0, height = 0;
    XtVaGetValues(widget, XmNwidth, &width, XmNheight, &height, NULL);
    return width > 0 && height > 0;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, form, title, text, button, scrolled, nested, row, dynamic;
    Widget inserted;
    Events events = {0, 0, 0, 0};
    Mutation mutation;
    Display *display;
    FILE *output;
    const char *output_path = argc > 1 ? argv[1] : "form-layout-corpus.txt";
    unsigned cycle;
    unsigned long no_op_geometry_requests;
    unsigned long no_op_geometry_queries;
    unsigned long no_op_configure;

    shell = XtVaAppInitialize(&app, "CkMotifFormLayoutCorpus", NULL, 0,
                              &argc, argv, NULL,
                              XmNwidth, 520, XmNheight, 360, NULL);
    display = XtDisplay(shell);
    form = XmCreateForm(shell, "form", NULL, 0);
    title = XmCreateLabel(form, "title", NULL, 0);
    text = XmCreateTextField(form, "text", NULL, 0);
    button = XmCreatePushButton(form, "mutate", NULL, 0);
    scrolled = XmCreateScrolledWindow(form, "scrolled", NULL, 0);
    nested = XmCreateForm(scrolled, "nested", NULL, 0);
    row = XmCreateRowColumn(nested, "row", NULL, 0);
    dynamic = XmCreateLabel(form, "dynamic", NULL, 0);
    XtVaSetValues(title, XmNtopAttachment, XmATTACH_FORM,
                  XmNleftAttachment, XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM, NULL);
    XtVaSetValues(text, XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, title, XmNleftAttachment, XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM, NULL);
    XtVaSetValues(button, XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, text, XmNleftAttachment, XmATTACH_FORM,
                  NULL);
    XtVaSetValues(dynamic, XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, text, XmNleftAttachment, XmATTACH_WIDGET,
                  XmNleftWidget, button, NULL);
    XtVaSetValues(scrolled, XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, button, XmNleftAttachment, XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM,
                  XmNbottomAttachment, XmATTACH_FORM, NULL);
    XtVaSetValues(nested, XmNwidth, 440, XmNheight, 230, NULL);
    XtVaSetValues(row, XmNorientation, XmVERTICAL, NULL);
    XtAddCallback(button, XmNactivateCallback, mutate_layout, &mutation);
    mutation.form = form;
    mutation.dynamic = dynamic;
    mutation.managed = True;
    mutation.activations = 0;
    XtManageChild(row);
    XmScrolledWindowSetAreas(scrolled, NULL, NULL, nested);
    XtManageChild(nested);
    XtManageChild(title);
    XtManageChild(text);
    XtManageChild(button);
    XtManageChild(dynamic);
    XtManageChild(scrolled);
    XtManageChild(form);
    watch(shell, &events);
    watch(form, &events);
    watch(scrolled, &events);
    watch(nested, &events);
    XtRealizeWidget(shell);
    XSync(display, False);
    drain(app);
    CHECK(valid_geometry(form));
    for (cycle = 0; cycle < 20; ++cycle) {
        XtVaSetValues(shell, XmNwidth, 360 + cycle * 23,
                      XmNheight, 240 + cycle * 11, NULL);
        XSync(display, False);
        drain(app);
        CHECK(valid_geometry(form));
    }
    XtUnmapWidget(shell);
    XSync(display, False);
    drain(app);
    XtMapWidget(shell);
    XSync(display, False);
    drain(app);
    CHECK(events.map > 0 && events.unmap > 0);
    for (cycle = 0; cycle < 4; ++cycle) {
        XEvent event;
        memset(&event, 0, sizeof(event));
        event.xkey.type = KeyPress;
        event.xkey.display = display;
        event.xkey.window = XtWindow(button);
        XtCallActionProc(button, "ArmAndActivate", &event, NULL, 0);
        XSync(display, False);
        drain(app);
    }
    CHECK(mutation.activations == 4);
    CHECK(mutation.managed == True);
    inserted = XmCreateLabel(form, "inserted", NULL, 0);
    XtVaSetValues(inserted, XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, dynamic, XmNleftAttachment, XmATTACH_FORM,
                  NULL);
    XtManageChild(inserted);
    XSync(display, False);
    drain(app);
    CHECK(XtIsManaged(inserted));
    CHECK(valid_geometry(inserted));
    XtDestroyWidget(inserted);
    XSync(display, False);
    drain(app);
    XtUnmanageChild(dynamic);
    XSync(display, False);
    drain(app);
    XtManageChild(dynamic);
    XSync(display, False);
    drain(app);
    CHECK(XtIsManaged(dynamic));
    CHECK(valid_geometry(dynamic));

    /* Negative control: reapplying the already active margin must not create
     * a geometry request or visible configure event.  This is deliberately
     * measured after all mutation work has settled. */
    {
        unsigned long before_requests = geometry_requests;
        unsigned long before_queries = geometry_queries;
        unsigned long before_configure = events.configure;
        for (cycle = 0; cycle < 5; ++cycle) {
            XtVaSetValues(form, XmNmarginWidth, 9, NULL);
            XSync(display, False);
            drain(app);
        }
        no_op_geometry_requests = geometry_requests - before_requests;
        no_op_geometry_queries = geometry_queries - before_queries;
        no_op_configure = events.configure - before_configure;
    }
    CHECK(no_op_geometry_requests == 0);
    CHECK(no_op_configure == 0);

    output = fopen(output_path, "w");
    CHECK(output != NULL);
    fprintf(output, "SCHEMA 1\n");
    fprintf(output, "GEOMETRY_REQUESTS %lu\n", geometry_requests);
    fprintf(output, "GEOMETRY_QUERIES %lu\n", geometry_queries);
    fprintf(output, "REDRAW_CLEAR_AREA %lu\n", clear_area_calls);
    fprintf(output, "REDRAW_COPY_AREA %lu\n", copy_area_calls);
    fprintf(output, "REDRAW_FILL_RECTANGLE %lu\n", fill_rectangle_calls);
    fprintf(output, "EVENT_CONFIGURE %lu\n", events.configure);
    fprintf(output, "EVENT_EXPOSE %lu\n", events.expose);
    fprintf(output, "EVENT_MAP %lu\n", events.map);
    fprintf(output, "EVENT_UNMAP %lu\n", events.unmap);
    fprintf(output, "CALLBACK_MUTATIONS %lu\n", mutation.activations);
    fprintf(output, "NOOP_GEOMETRY_REQUESTS %lu\n", no_op_geometry_requests);
    fprintf(output, "NOOP_GEOMETRY_QUERIES %lu\n", no_op_geometry_queries);
    fprintf(output, "NOOP_CONFIGURE %lu\n", no_op_configure);
    fclose(output);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS Form layout diagnostic corpus: %lu checks\n", checks);
    return 0;
}
