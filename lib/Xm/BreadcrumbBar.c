#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/BreadcrumbBarP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);

#define Offset(field) XtOffsetOf(XmBreadcrumbBarRec, field)
static XtResource resources[] = {
    {XmNbreadcrumbMaxVisible, XmCBreadcrumbMaxVisible, XmRInt, sizeof(int),
     Offset(breadcrumb.max_visible), XmRImmediate, (XtPointer)0},
    {XmNbreadcrumbSeparator, XmCBreadcrumbSeparator, XmRString, sizeof(String),
     Offset(breadcrumb.separator), XmRImmediate, (XtPointer)"/"},
    {XmNbreadcrumbRtl, XmCBreadcrumbRtl, XmRBoolean, sizeof(Boolean),
     Offset(breadcrumb.rtl), XmRImmediate, (XtPointer)False},
    {XmNbreadcrumbActivateCallback, XmCBreadcrumbActivateCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(breadcrumb.activate_callback), XmRPointer, (XtPointer)NULL},
    {XmNbreadcrumbCurrentChangedCallback, XmCBreadcrumbCurrentChangedCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(breadcrumb.current_changed_callback), XmRPointer, (XtPointer)NULL},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmbreadcrumbbarclassrec) XmBreadcrumbBarClassRec
xmBreadcrumbBarClassRec = {{(WidgetClass)&xmPrimitiveClassRec,
    "XmBreadcrumbBar", sizeof(XmBreadcrumbBarRec), NULL, NULL, False,
    Initialize, NULL, XtInheritRealize, NULL, 0, resources,
    XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True,
    False, Destroy, NULL, Redisplay, SetValues, NULL,
    XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
    XtInheritTranslations,
    QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}};

externaldef(xmbreadcrumbbarwidgetclass) WidgetClass
xmBreadcrumbBarWidgetClass = (WidgetClass)&xmBreadcrumbBarClassRec;

static void free_items(XmBreadcrumbBarWidget bar) {
    Cardinal i;
    for (i = 0; i < bar->breadcrumb.count; ++i)
        XtFree(bar->breadcrumb.items[i].label);
    XtFree((char *)bar->breadcrumb.items);
    bar->breadcrumb.items = NULL;
    bar->breadcrumb.count = 0;
}

static void notify_current(XmBreadcrumbBarWidget bar, XEvent *event) {
    XmBreadcrumbBarCallbackStruct cb;
    if (!bar->breadcrumb.current_changed_callback ||
        bar->breadcrumb.current < 0 ||
        (Cardinal)bar->breadcrumb.current >= bar->breadcrumb.count)
        return;
    cb.reason = 1;
    cb.event = event;
    cb.index = (Cardinal)bar->breadcrumb.current;
    cb.id = bar->breadcrumb.items[cb.index].id;
    XtCallCallbacks((Widget)bar, XmNbreadcrumbCurrentChangedCallback, &cb);
}

static void activate(XmBreadcrumbBarWidget bar, XEvent *event) {
    XmBreadcrumbBarCallbackStruct cb;
    if (!bar->breadcrumb.activate_callback ||
        bar->breadcrumb.current < 0 ||
        (Cardinal)bar->breadcrumb.current >= bar->breadcrumb.count)
        return;
    cb.reason = 2;
    cb.event = event;
    cb.index = (Cardinal)bar->breadcrumb.current;
    cb.id = bar->breadcrumb.items[cb.index].id;
    XtCallCallbacks((Widget)bar, XmNbreadcrumbActivateCallback, &cb);
}

static void set_current(XmBreadcrumbBarWidget bar, int index,
                        Boolean notify, XEvent *event) {
    if (index < 0 || (Cardinal)index >= bar->breadcrumb.count ||
        index == bar->breadcrumb.current)
        return;
    bar->breadcrumb.current = index;
    if (XtIsRealized((Widget)bar))
        XClearArea(XtDisplay((Widget)bar), XtWindow((Widget)bar), 0, 0, 0, 0,
                  True);
    if (notify)
        notify_current(bar, event);
}

static int item_width(XmBreadcrumbBarWidget bar, Cardinal index) {
    const char *label = bar->breadcrumb.items[index].label ?
                        bar->breadcrumb.items[index].label : "";
    const char *separator = bar->breadcrumb.separator ?
                            bar->breadcrumb.separator : "";
    int width = (int)strlen(label) * 8 + 10;
    if (index)
        width += (int)strlen(separator) * 8 + 8;
    return width;
}

static Cardinal visible_start(XmBreadcrumbBarWidget bar) {
    if (bar->breadcrumb.max_visible <= 0 ||
        bar->breadcrumb.count <= (Cardinal)bar->breadcrumb.max_visible)
        return 0;
    return bar->breadcrumb.count - (Cardinal)bar->breadcrumb.max_visible + 1;
}

static int hit_test(XmBreadcrumbBarWidget bar, int x) {
    Cardinal i, start = visible_start(bar);
    int cursor = 4;
    if (start) {
        if (x < cursor + 24)
            return -1;
        cursor += 28;
    }
    if (!bar->breadcrumb.rtl) {
        for (i = start; i < bar->breadcrumb.count; ++i) {
            int width = item_width(bar, i);
            if (x >= cursor && x < cursor + width)
                return (int)i;
            cursor += width;
        }
    } else {
        for (i = bar->breadcrumb.count; i-- > start; ) {
            int width = item_width(bar, i);
            if (x >= cursor && x < cursor + width)
                return (int)i;
            cursor += width;
        }
    }
    return -1;
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmBreadcrumbBarWidget bar = (XmBreadcrumbBarWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
    bar->breadcrumb.items = NULL;
    bar->breadcrumb.count = 0;
    bar->breadcrumb.current = -1;
    values.foreground = bar->primitive.foreground;
    values.background = bar->core.background_pixel;
    bar->breadcrumb.gc = XCreateGC(XtDisplay(widget),
                                   DefaultRootWindow(XtDisplay(widget)),
                                   GCForeground | GCBackground, &values);
    if (!bar->core.width)
        bar->core.width = 240;
    if (!bar->core.height)
        bar->core.height = 28;
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask | ButtonReleaseMask,
                      False, Events, NULL);
}

static void Destroy(Widget widget) {
    XmBreadcrumbBarWidget bar = (XmBreadcrumbBarWidget)widget;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask |
                         ButtonReleaseMask, False, Events, NULL);
    free_items(bar);
    if (bar->breadcrumb.gc)
        XFreeGC(XtDisplay(widget), bar->breadcrumb.gc);
}

static void draw_item(XmBreadcrumbBarWidget bar, Cardinal index, int *cursor,
                      Boolean first) {
    Display *display = XtDisplay((Widget)bar);
    Window window = XtWindow((Widget)bar);
    const char *label = bar->breadcrumb.items[index].label ?
                        bar->breadcrumb.items[index].label : "";
    const char *separator = bar->breadcrumb.separator ?
                            bar->breadcrumb.separator : "";
    int width = item_width(bar, index);
    int text_x = *cursor + (first ? 0 : 4);
    if (!first)
        XDrawString(display, window, bar->breadcrumb.gc, *cursor, 18,
                    separator, (int)strlen(separator));
    if ((int)index == bar->breadcrumb.current)
        XFillRectangle(display, window, bar->breadcrumb.gc, text_x - 2, 5,
                       (unsigned)(width > 4 ? width - 4 : 1), 19);
    XDrawString(display, window, bar->breadcrumb.gc, text_x, 18,
                label, (int)strlen(label));
    *cursor += width;
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmBreadcrumbBarWidget bar = (XmBreadcrumbBarWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    Cardinal i, start = visible_start(bar);
    int cursor = 4;
    (void)event; (void)region;
    if (!window || !bar->breadcrumb.gc)
        return;
    XClearWindow(display, window);
    if (start) {
        XDrawString(display, window, bar->breadcrumb.gc, cursor, 18, "...", 3);
        cursor += 28;
    }
    if (!bar->breadcrumb.rtl) {
        for (i = start; i < bar->breadcrumb.count; ++i)
            draw_item(bar, i, &cursor, i == start && !start);
    } else {
        for (i = bar->breadcrumb.count; i-- > start; )
            draw_item(bar, i, &cursor, i == bar->breadcrumb.count - 1);
    }
    if (bar->primitive.highlighted && bar->primitive.highlight_thickness &&
        bar->core.width > 2 * bar->primitive.highlight_thickness &&
        bar->core.height > 2 * bar->primitive.highlight_thickness)
        XDrawRectangle(display, window, bar->breadcrumb.gc,
                       bar->primitive.highlight_thickness - 1,
                       bar->primitive.highlight_thickness - 1,
                       bar->core.width - 2 * bar->primitive.highlight_thickness,
                       bar->core.height - 2 * bar->primitive.highlight_thickness);
}

static void Events(Widget widget, XtPointer data, XEvent *event,
                   Boolean *dispatch) {
    XmBreadcrumbBarWidget bar = (XmBreadcrumbBarWidget)widget;
    KeySym key;
    char buffer[2];
    int length, index;
    (void)data; (void)dispatch;
    if (event->type == ButtonPress) {
        index = hit_test(bar, event->xbutton.x);
        if (index >= 0)
            set_current(bar, index, True, event);
        return;
    }
    if (event->type == ButtonRelease) {
        if (hit_test(bar, event->xbutton.x) >= 0)
            activate(bar, event);
        return;
    }
    if (event->type != KeyPress)
        return;
    length = XLookupString(&event->xkey, buffer, sizeof(buffer), &key, NULL);
    (void)length;
    if (key == XK_Return || key == XK_KP_Enter) {
        activate(bar, event);
    } else if (key == XK_Left) {
        set_current(bar, bar->breadcrumb.current + (bar->breadcrumb.rtl ? 1 : -1),
                    True, event);
    } else if (key == XK_Right) {
        set_current(bar, bar->breadcrumb.current + (bar->breadcrumb.rtl ? -1 : 1),
                    True, event);
    }
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmBreadcrumbBarWidget old = (XmBreadcrumbBarWidget)old_widget;
    XmBreadcrumbBarWidget bar = (XmBreadcrumbBarWidget)widget;
    (void)request; (void)args; (void)count;
    return old->breadcrumb.max_visible != bar->breadcrumb.max_visible ||
           old->breadcrumb.rtl != bar->breadcrumb.rtl ||
           old->breadcrumb.separator != bar->breadcrumb.separator;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmBreadcrumbBarWidget bar = (XmBreadcrumbBarWidget)widget;
    Cardinal i;
    int width = 8;
    (void)request;
    for (i = 0; i < bar->breadcrumb.count; ++i)
        width += item_width(bar, i);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = (Dimension)(width > 240 ? 240 : width);
    reply->height = 28;
    return XtGeometryAlmost;
}

Widget XmCreateBreadcrumbBar(Widget parent, char *name, ArgList args,
                             Cardinal count) {
    return XtCreateWidget(name, xmBreadcrumbBarWidgetClass, parent, args, count);
}

void XmBreadcrumbBarSetSegments(Widget widget,
                                const XmBreadcrumbSegment *segments,
                                Cardinal count) {
    XmBreadcrumbBarWidget bar;
    Cardinal i;
    if (!XmIsBreadcrumbBar(widget))
        return;
    bar = (XmBreadcrumbBarWidget)widget;
    free_items(bar);
    if (count) {
        bar->breadcrumb.items = (XmBreadcrumbBarItem *)XtCalloc(count,
                                                                  sizeof(*bar->breadcrumb.items));
        for (i = 0; i < count; ++i) {
            bar->breadcrumb.items[i].label = XtNewString(
                segments && segments[i].label ? segments[i].label : "");
            bar->breadcrumb.items[i].id = segments ? segments[i].id : NULL;
        }
        bar->breadcrumb.count = count;
        if (bar->breadcrumb.current < 0 ||
            (Cardinal)bar->breadcrumb.current >= count)
            bar->breadcrumb.current = (int)count - 1;
    } else {
        bar->breadcrumb.current = -1;
    }
    if (XtIsRealized(widget))
        XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
}

Cardinal XmBreadcrumbBarGetSegmentCount(Widget widget) {
    return XmIsBreadcrumbBar(widget) ?
        ((XmBreadcrumbBarWidget)widget)->breadcrumb.count : 0;
}

int XmBreadcrumbBarGetCurrentIndex(Widget widget) {
    return XmIsBreadcrumbBar(widget) ?
        ((XmBreadcrumbBarWidget)widget)->breadcrumb.current : -1;
}

void XmBreadcrumbBarSetCurrentIndex(Widget widget, int index,
                                    Boolean notify_change) {
    if (XmIsBreadcrumbBar(widget))
        set_current((XmBreadcrumbBarWidget)widget, index, notify_change, NULL);
}
