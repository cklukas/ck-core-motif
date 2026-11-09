#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Xm/XmP.h>
#include "PaintI.h"
#include "XmI.h"
#ifdef USE_XFT
#include "XmRenderTI.h"
#endif
#include <string.h>
#ifdef HAVE_X11_EXTENSIONS_XDBE_H
#include <X11/extensions/Xdbe.h>
#endif

/* State lives outside widget records: no private-record ABI change. All entry
 * points run under the Xt application lock; the process lock protects the list. */
typedef struct PaintState {
    Widget widget;
    Window window;
    Drawable buffer, active;
    GC gc;
    Dimension width, height;
    unsigned int depth;
    Boolean dbe;
    XtIntervalId pending;
    String backend;
    struct PaintState *next;
} PaintState;
static PaintState *states;
static Display *allocation_display;
static unsigned long allocation_serial;
static int allocation_error;
static XErrorHandler previous_handler;

static int allocation_failed(Display *display, XErrorEvent *event)
{
    if (display == allocation_display && event->serial == allocation_serial) {
        allocation_error = event->error_code;
        return 0;
    }
    return previous_handler ? previous_handler(display, event) : 0;
}

/* X allocation errors are asynchronous. Scope the trap to this exact request;
 * synchronize before installing it, and preserve the application's handler. */
static void begin_allocation(Display *display)
{
    XLockDisplay(display);
    XSync(display, False);
    allocation_display = display;
    allocation_serial = NextRequest(display);
    allocation_error = 0;
    previous_handler = XSetErrorHandler(allocation_failed);
}
static Boolean end_allocation(Display *display)
{
    XSync(display, False);
    XSetErrorHandler(previous_handler);
    allocation_display = NULL;
    XUnlockDisplay(display);
    return allocation_error == 0;
}

static void release_buffer(PaintState *state)
{
    Display *display = XtDisplay(state->widget);
    /* DestroyWindow already releases DBE resources on an unrealized window. */
    if (state->buffer) {
#ifdef USE_XFT
        _XmXftDrawEvict(display, state->buffer);
#endif
#ifdef HAVE_X11_EXTENSIONS_XDBE_H
        if (state->dbe) {
            if (XtWindow(state->widget) == state->window)
                XdbeDeallocateBackBufferName(display, state->buffer);
        } else
#endif
            XFreePixmap(display, state->buffer);
    }
    state->buffer = None;
    state->dbe = False;
}
static void destroyed(Widget widget, XtPointer client, XtPointer call)
{
    PaintState *state = (PaintState *)client, **link;
    (void)widget; (void)call;
    _XmProcessLock();
    for (link = &states; *link && *link != state; link = &(*link)->next) {}
    if (*link) *link = state->next;
    if (state->pending) XtRemoveTimeOut(state->pending);
    release_buffer(state);
    if (state->gc) XFreeGC(XtDisplay(state->widget), state->gc);
    XtFree(state->backend);
    XtFree((char *)state);
    _XmProcessUnlock();
}
static PaintState *find_state(Widget widget)
{
    PaintState *state;
    for (state = states; state; state = state->next)
        if (state->widget == widget) return state;
    return NULL;
}
int _XmPaintBufferKind(Widget widget)
{
    int kind = 0;
    _XmProcessLock();
    PaintState *state = find_state(widget);
    if (state && state->buffer) kind = state->dbe ? 2 : 1;
    _XmProcessUnlock();
    return kind;
}
Drawable _XmPaintDrawable(Widget widget)
{
    Drawable result;
    PaintState *state;
    if (!widget) return None;
    _XmProcessLock();
    state = find_state(widget);
    result = state && state->active ? state->active : XtWindow(widget);
    _XmProcessUnlock();
    return result;
}

static void allocate_buffer(PaintState *state)
{
    Widget widget = state->widget;
    Display *display = XtDisplay(widget);
    Boolean ok;
    if (!strcmp(state->backend, "direct")) return;
#ifdef HAVE_X11_EXTENSIONS_XDBE_H
    if (!strcmp(state->backend, "auto")) {
        int major, minor, screens = 1, i;
        Drawable window = state->window;
        if (XdbeQueryExtension(display, &major, &minor)) {
            XdbeScreenVisualInfo *info = XdbeGetVisualInfo(display, &window, &screens);
            XWindowAttributes attributes;
            XGetWindowAttributes(display, window, &attributes);
            VisualID visual = XVisualIDFromVisual(attributes.visual);
            Boolean supported = False;
            if (info) {
                for (i = 0; i < info[0].count; ++i)
                    if (info[0].visinfo[i].visual == visual) supported = True;
                XdbeFreeVisualInfo(info);
            }
            if (supported) {
                begin_allocation(display);
                state->buffer = XdbeAllocateBackBufferName(display, window, XdbeUndefined);
                ok = end_allocation(display);
                if (ok && state->buffer) {
                    state->dbe = True;
                    return;
                }
                state->buffer = None;
            }
        }
    }
#endif
    begin_allocation(display);
    state->buffer = XCreatePixmap(display, state->window, widget->core.width,
                                  widget->core.height, widget->core.depth);
    ok = end_allocation(display);
    if (!ok) state->buffer = None;
}

static void background(PaintState *state)
{
    Widget widget = state->widget, owner = widget;
    int x = 0, y = 0;
    XGCValues values;
    unsigned long mask = GCForeground | GCFillStyle | GCGraphicsExposures;
    values.foreground = widget->core.background_pixel;
    values.fill_style = FillSolid;
    values.graphics_exposures = False;
    while (owner->core.background_pixmap == ParentRelative && XtParent(owner)) {
        x -= owner->core.x + owner->core.border_width;
        y -= owner->core.y + owner->core.border_width;
        owner = XtParent(owner);
    }
    if (owner->core.background_pixmap != None &&
        owner->core.background_pixmap != ParentRelative &&
        owner->core.background_pixmap != XtUnspecifiedPixmap) {
        values.tile = owner->core.background_pixmap;
        values.fill_style = FillTiled;
        values.ts_x_origin = x;
        values.ts_y_origin = y;
        mask |= GCTile | GCTileStipXOrigin | GCTileStipYOrigin;
    } else if (widget->core.background_pixmap == ParentRelative)
        values.foreground = owner->core.background_pixel;
    XChangeGC(XtDisplay(widget), state->gc, mask, &values);
    XFillRectangle(XtDisplay(widget), state->active, state->gc, 0, 0,
                   widget->core.width, widget->core.height);
}

static PaintState *ensure_state(Widget widget)
{
    PaintState *state = find_state(widget);
    static XtResource resources[] = {
        {"paintBuffer", "PaintBuffer", XtRString, sizeof(String),
         XtOffsetOf(PaintState, backend), XtRString, (XtPointer)"auto"}
    };
    if (!state) {
        state = (PaintState *)XtCalloc(1, sizeof(*state));
        if (!state)
            return NULL;
        state->widget = widget;
        XtGetApplicationResources(widget, state, resources, XtNumber(resources), NULL, 0);
        state->backend = XtNewString(state->backend ? state->backend : "auto");
        if (!state->backend) {
            XtFree((char *)state);
            return NULL;
        }
        state->next = states;
        states = state;
        XtAddCallback(widget, XtNdestroyCallback, destroyed, state);
    }
    return state;
}
static void repaint(XtPointer client, XtIntervalId *timer)
{
    PaintState *state = (PaintState *)client;
    Widget widget = state->widget;
    (void)timer;
    state->pending = 0;
    if (XtIsRealized(widget) && !widget->core.being_destroyed &&
        XtClass(widget)->core_class.expose)
        XtClass(widget)->core_class.expose(widget, NULL, NULL);
}
void _XmPaintInvalidate(Widget widget)
{
    PaintState *state;
    if (!widget || !XtIsRealized(widget) || widget->core.being_destroyed) return;
    _XmProcessLock();
    state = ensure_state(widget);
    if (!state) {
        _XmProcessUnlock();
        return;
    }
    if (!state->pending)
        state->pending = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), 0, repaint, state);
    _XmProcessUnlock();
}

void _XmBufferedPaint(Widget widget, XtExposeProc paint, XEvent *event, Region region)
{
    PaintState *state;
    Display *display;
    if (!widget || !paint || !XtIsWidget(widget) || widget->core.being_destroyed ||
        !XtIsRealized(widget) || !widget->core.width || !widget->core.height) return;
    display = XtDisplay(widget);
    _XmProcessLock();
    state = ensure_state(widget);
    if (!state) {
        _XmProcessUnlock();
        return;
    }
    if (state->pending) {
        XtRemoveTimeOut(state->pending);
        state->pending = 0;
    }
    /* Nested painting of the same widget shares the outer transaction. */
    if (state->active) {
        paint(widget, event, region);
        _XmProcessUnlock();
        return;
    }
    Boolean same_window = state->window == XtWindow(widget) && state->depth == widget->core.depth;
    if (!same_window || state->width != widget->core.width || state->height != widget->core.height) {
        /* DBE follows native ConfigureWindow sizes automatically. Pixmaps
         * need replacement, and a newly realized window needs fresh resources. */
        if (!same_window || !state->dbe) {
            release_buffer(state);
            state->window = XtWindow(widget);
            state->depth = widget->core.depth;
            if (state->gc) XFreeGC(display, state->gc);
            state->gc = XCreateGC(display, state->window, 0, NULL);
            if (state->gc)
                allocate_buffer(state);
        }
        state->width = widget->core.width;
        state->height = widget->core.height;
    }
    if (!state->gc)
        state->gc = XCreateGC(display, state->window, 0, NULL);
    if (!state->gc) {
        _XmProcessUnlock();
        return;
    }
    state->active = state->buffer ? state->buffer : state->window;
    background(state);
    /* These painters repaint the whole widget, including the background. */
    paint(widget, event, NULL);
    if (state->buffer) {
#ifdef HAVE_X11_EXTENSIONS_XDBE_H
        if (state->dbe) {
#ifdef USE_XFT
            _XmXftDrawEvict(display, state->buffer);
#endif
            XdbeSwapInfo swap = {state->window, XdbeUndefined};
            XdbeSwapBuffers(display, &swap, 1);
        } else
#endif
            XCopyArea(display, state->buffer, state->window, state->gc,
                      0, 0, state->width, state->height, 0, 0);
    }
    state->active = None;
    _XmProcessUnlock();
}
