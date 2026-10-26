#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <limits.h>
#include <stdint.h>
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
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmBreadcrumbBarWidget bar = (XmBreadcrumbBarWidget)widget;
    (void)offset;
    if (bar->breadcrumb.check_set_render_table)
        value->addr = NULL;
    else {
        bar->breadcrumb.check_set_render_table = True;
        value->addr = (char *)&bar->breadcrumb.font;
    }
}
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
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(breadcrumb.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmFontList),
     Offset(breadcrumb.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
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
    size_t label_len = strlen(label), separator_len = strlen(separator);
    int width;
    if (label_len > (size_t)(INT_MAX - 10) / 8)
        width = INT_MAX;
    else
        width = (int)(label_len * 8) + 10;
    if (index) {
        if (separator_len > (size_t)(INT_MAX - 8) / 8 ||
            width > INT_MAX - (int)(separator_len * 8) - 8)
            width = INT_MAX;
        else
            width += (int)(separator_len * 8) + 8;
    }
    return width;
}

static void advance_cursor(int *cursor, int width) {
    if (*cursor >= INT_MAX - width)
        *cursor = INT_MAX;
    else
        *cursor += width;
}

static int bounded_text_length(const char *text) {
    size_t length = text ? strlen(text) : 0;
    return length > (size_t)INT_MAX ? INT_MAX : (int)length;
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
            if (x >= cursor && (width == INT_MAX || x - cursor < width))
                return (int)i;
            advance_cursor(&cursor, width);
        }
    } else {
        for (i = bar->breadcrumb.count; i-- > start; ) {
            int width = item_width(bar, i);
            if (x >= cursor && (width == INT_MAX || x - cursor < width))
                return (int)i;
            advance_cursor(&cursor, width);
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
    if (!bar->breadcrumb.font)
        bar->breadcrumb.font = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        bar->breadcrumb.font = XmFontListCopy(bar->breadcrumb.font);
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
    if (bar->breadcrumb.font)
        XmRenderTableFree(bar->breadcrumb.font);
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
    int text_x;
    XmString label_string = XmStringCreateLocalized((char *)label);
    XmString separator_string = XmStringCreateLocalized((char *)separator);
    Dimension label_width = 0, label_height = 0, separator_width = 0;
    int text_y = 0;
    if (bar->breadcrumb.font && label_string)
        XmStringExtent(bar->breadcrumb.font, label_string, &label_width,
                       &label_height);
    if (bar->breadcrumb.font && separator_string) {
        Dimension separator_height = 0;
        XmStringExtent(bar->breadcrumb.font, separator_string, &separator_width,
                       &separator_height);
    }
    text_x = first ? *cursor :
        (separator_width > (Dimension)(INT_MAX - *cursor - 4)
             ? INT_MAX : *cursor + (int)separator_width + 4);
    text_y = ((int)bar->core.height - (int)label_height) / 2;
    if (!first && separator_string)
        XmStringDraw(display, window, bar->breadcrumb.font, separator_string,
                     bar->breadcrumb.gc, *cursor, text_y, separator_width,
                     XmALIGNMENT_BEGINNING, XmPrim_layout_direction(bar), NULL);
    if ((int)index == bar->breadcrumb.current)
        XFillRectangle(display, window, bar->breadcrumb.gc, text_x - 2, 5,
                       (unsigned)(label_width > 4 ? label_width + 4 : 1),
                       (unsigned)(bar->core.height > 10 ? bar->core.height - 10 : 1));
    if ((int)index == bar->breadcrumb.current)
        XSetForeground(display, bar->breadcrumb.gc, bar->core.background_pixel);
    if (label_string)
        XmStringDraw(display, window, bar->breadcrumb.font, label_string,
                     bar->breadcrumb.gc, text_x, text_y, label_width,
                     XmALIGNMENT_BEGINNING, XmPrim_layout_direction(bar), NULL);
    if ((int)index == bar->breadcrumb.current)
        XSetForeground(display, bar->breadcrumb.gc, bar->primitive.foreground);
    XmStringFree(label_string);
    XmStringFree(separator_string);
    advance_cursor(cursor, width);
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
        XmString ellipsis = XmStringCreateLocalized("...");
        Dimension ellipsis_width = 0, ellipsis_height = 0;
        if (ellipsis && bar->breadcrumb.font) {
            XmStringExtent(bar->breadcrumb.font, ellipsis,
                           &ellipsis_width, &ellipsis_height);
            XmStringDraw(display, window, bar->breadcrumb.font, ellipsis,
                         bar->breadcrumb.gc, cursor,
                         ((int)bar->core.height - (int)ellipsis_height) / 2,
                         ellipsis_width, XmALIGNMENT_BEGINNING,
                         XmPrim_layout_direction(bar), NULL);
        }
        XmStringFree(ellipsis);
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
    Boolean redraw = False;
    (void)request; (void)args; (void)count;
    if (old->breadcrumb.font != bar->breadcrumb.font) {
        XmFontList replacement = bar->breadcrumb.font
            ? XmFontListCopy(bar->breadcrumb.font)
            : XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (old->breadcrumb.font) XmRenderTableFree(old->breadcrumb.font);
        bar->breadcrumb.font = replacement;
        redraw = True;
    }
    return redraw || old->breadcrumb.max_visible != bar->breadcrumb.max_visible ||
           old->breadcrumb.rtl != bar->breadcrumb.rtl ||
           old->breadcrumb.separator != bar->breadcrumb.separator;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmBreadcrumbBarWidget bar = (XmBreadcrumbBarWidget)widget;
    Cardinal i;
    int width = 8;
    (void)request;
    for (i = 0; i < bar->breadcrumb.count; ++i) {
        int item = item_width(bar, i);
        if (width >= INT_MAX - item) {
            width = INT_MAX;
            break;
        }
        width += item;
    }
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
        if ((size_t)count > SIZE_MAX / sizeof(*bar->breadcrumb.items) ||
            (size_t)count > UINT_MAX / sizeof(*bar->breadcrumb.items))
        {
            bar->breadcrumb.current = -1;
            return;
        }
        bar->breadcrumb.items = (XmBreadcrumbBarItem *)XtCalloc(
            (Cardinal)count, sizeof(*bar->breadcrumb.items));
        if (!bar->breadcrumb.items) {
            bar->breadcrumb.current = -1;
            return;
        }
        for (i = 0; i < count; ++i) {
            bar->breadcrumb.items[i].label = XtNewString(
                segments && segments[i].label ? segments[i].label : "");
            if (!bar->breadcrumb.items[i].label) {
                while (i != 0) {
                    --i;
                    XtFree(bar->breadcrumb.items[i].label);
                }
                XtFree((char *)bar->breadcrumb.items);
                bar->breadcrumb.items = NULL;
                bar->breadcrumb.current = -1;
                return;
            }
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
