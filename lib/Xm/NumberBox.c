/* Additive integer NumberBox.  Text is edited locally and committed only
 * after complete parsing; existing XmTextField/XmSpinBox records are untouched. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/NumberBoxP.h>
#include <Xm/VaSimpleP.h>
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

#define Offset(field) XtOffsetOf(XmNumberBoxRec, field)
static XtResource resources[] = {
    {XmNnumberMinimum, XmCNumberMinimum, XmRInt, sizeof(int),
     Offset(number_box.minimum), XmRImmediate, (XtPointer)0},
    {XmNnumberMaximum, XmCNumberMaximum, XmRInt, sizeof(int),
     Offset(number_box.maximum), XmRImmediate, (XtPointer)100},
    {XmNnumberValue, XmCNumberValue, XmRInt, sizeof(int),
     Offset(number_box.value), XmRImmediate, (XtPointer)0},
    {XmNnumberStep, XmCNumberStep, XmRInt, sizeof(int),
     Offset(number_box.step), XmRImmediate, (XtPointer)1},
    {XmNnumberPageStep, XmCNumberPageStep, XmRInt, sizeof(int),
     Offset(number_box.page_step), XmRImmediate, (XtPointer)10},
    {XmNnumberWrap, XmCNumberWrap, XmRBoolean, sizeof(Boolean),
     Offset(number_box.wrap), XmRImmediate, (XtPointer)False},
    {XmNnumberValueChangedCallback, XmCNumberValueChangedCallback,
     XmRCallback, sizeof(XtCallbackList), Offset(number_box.value_changed_callback),
     XmRPointer, (XtPointer)NULL},
    {XmNnumberInvalidCallback, XmCNumberInvalidCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(number_box.invalid_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
     Offset(number_box.spacing), XmRImmediate, (XtPointer)6},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmnumberboxclassrec) XmNumberBoxClassRec xmNumberBoxClassRec = {
    {
        (WidgetClass)&xmPrimitiveClassRec, "XmNumberBox", sizeof(XmNumberBoxRec),
        NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
        resources, XtNumber(resources), NULLQUARK, True,
        XtExposeCompressMaximal, True, False, Destroy, NULL, Redisplay,
        SetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion,
        NULL, XtInheritTranslations, QueryGeometry, NULL, NULL
    },
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}
};
externaldef(xmnumberboxwidgetclass) WidgetClass xmNumberBoxWidgetClass =
    (WidgetClass)&xmNumberBoxClassRec;

static void normalize(XmNumberBoxWidget number) {
    if (number->number_box.maximum < number->number_box.minimum)
        number->number_box.maximum = number->number_box.minimum;
    if (number->number_box.value < number->number_box.minimum)
        number->number_box.value = number->number_box.minimum;
    if (number->number_box.value > number->number_box.maximum)
        number->number_box.value = number->number_box.maximum;
    if (number->number_box.step < 1) number->number_box.step = 1;
    if (number->number_box.page_step < 1)
        number->number_box.page_step = number->number_box.step;
}

static void format_value(XmNumberBoxWidget number) {
    (void)snprintf(number->number_box.text, sizeof(number->number_box.text),
                   "%d", number->number_box.value);
}

static Boolean parse_text(const char *text, int *value) {
    char *end;
    long parsed;
    if (!text || !*text) return False;
    errno = 0;
    parsed = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end || parsed < INT_MIN ||
        parsed > INT_MAX)
        return False;
    *value = (int)parsed;
    return True;
}

static void redraw(XmNumberBoxWidget number) {
    if (XtIsRealized((Widget)number))
        XClearArea(XtDisplay((Widget)number), XtWindow((Widget)number),
                   0, 0, 0, 0, True);
}

static void notify_value(XmNumberBoxWidget number, XEvent *event) {
    XmNumberBoxCallbackStruct callback;
    callback.reason = XmCR_VALUE_CHANGED;
    callback.event = event;
    callback.minimum = number->number_box.minimum;
    callback.maximum = number->number_box.maximum;
    callback.value = number->number_box.value;
    callback.step = number->number_box.step;
    callback.page_step = number->number_box.page_step;
    XtCallCallbacks((Widget)number, XmNnumberValueChangedCallback, &callback);
}

static void notify_invalid(XmNumberBoxWidget number, XEvent *event) {
    XmNumberBoxInvalidCallbackStruct callback;
    callback.reason = XmCR_VALUE_CHANGED;
    callback.event = event;
    callback.text = number->number_box.text;
    XtCallCallbacks((Widget)number, XmNnumberInvalidCallback, &callback);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmNumberBoxWidget number = (XmNumberBoxWidget)widget;
    Display *display = XtDisplay(widget);
    XGCValues values;
    unsigned long mask = GCForeground | GCBackground;
    (void)request; (void)args; (void)count;
    normalize(number);
    format_value(number);
    values.foreground = number->primitive.foreground;
    values.background = number->core.background_pixel;
    number->number_box.text_gc = XCreateGC(display, DefaultRootWindow(display),
                                           mask, &values);
    values.foreground = number->primitive.foreground;
    number->number_box.arrow_gc = XCreateGC(display, DefaultRootWindow(display),
                                            mask, &values);
    values.foreground = number->primitive.bottom_shadow_color;
    number->number_box.invalid_gc = XCreateGC(display, DefaultRootWindow(display),
                                              mask, &values);
    if (!number->core.width) number->core.width = 140;
    if (!number->core.height) number->core.height = 32;
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events,
                      NULL);
}

static void Destroy(Widget widget) {
    XmNumberBoxWidget number = (XmNumberBoxWidget)widget;
    Display *display = XtDisplay(widget);
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False,
                         Events, NULL);
    if (number->number_box.text_gc) XFreeGC(display, number->number_box.text_gc);
    if (number->number_box.arrow_gc) XFreeGC(display, number->number_box.arrow_gc);
    if (number->number_box.invalid_gc) XFreeGC(display, number->number_box.invalid_gc);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmNumberBoxWidget number = (XmNumberBoxWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    XFontStruct *font;
    int baseline;
    Position arrow_x;
    XPoint up[3], down[3];
    GC text_gc;
    (void)event; (void)region;
    if (!window || !number->number_box.text_gc ||
        !number->number_box.arrow_gc)
        return;
    text_gc = number->number_box.invalid ? number->number_box.invalid_gc
                                         : number->number_box.text_gc;
    if (!text_gc)
        return;
    XClearWindow(display, window);
    font = XQueryFont(display, XGContextFromGC(number->number_box.text_gc));
    if (!font)
        return;
    baseline = (int)((number->core.height + font->ascent - font->descent) / 2);
    XDrawString(display, window, text_gc, (int)number->number_box.spacing,
                baseline, number->number_box.text,
                (int)strlen(number->number_box.text));
    arrow_x = number->core.width > 24 ? number->core.width - 24 : 0;
    XDrawLine(display, window, number->number_box.arrow_gc, arrow_x, 0,
              arrow_x, number->core.height);
    up[0].x = (short)(arrow_x + 12); up[0].y = 5;
    up[1].x = (short)(arrow_x + 5); up[1].y = (short)(number->core.height / 2 - 2);
    up[2].x = (short)(arrow_x + 19); up[2].y = up[1].y;
    down[0].x = up[1].x; down[0].y = (short)(number->core.height / 2 + 2);
    down[1].x = up[2].x; down[1].y = down[0].y;
    down[2].x = up[0].x; down[2].y = (short)(number->core.height - 5);
    XFillPolygon(display, window, number->number_box.arrow_gc, up, 3,
                 Convex, CoordModeOrigin);
    XFillPolygon(display, window, number->number_box.arrow_gc, down, 3,
                 Convex, CoordModeOrigin);
    if (font) XFreeFont(display, font);
    if (number->primitive.highlighted && number->primitive.highlight_thickness)
        XDrawRectangle(display, window, number->number_box.arrow_gc,
                       number->primitive.highlight_thickness - 1,
                       number->primitive.highlight_thickness - 1,
                       number->core.width - 2 * number->primitive.highlight_thickness,
                       number->core.height - 2 * number->primitive.highlight_thickness);
}

static void commit_text(XmNumberBoxWidget number, XEvent *event) {
    int value;
    if (!parse_text(number->number_box.text, &value) ||
        value < number->number_box.minimum || value > number->number_box.maximum) {
        number->number_box.invalid = True;
        redraw(number);
        notify_invalid(number, event);
        return;
    }
    number->number_box.invalid = False;
    if (number->number_box.value != value) {
        number->number_box.value = value;
        redraw(number);
        notify_value(number, event);
    } else redraw(number);
}

static void step_value(XmNumberBoxWidget number, int direction, Boolean page,
                       Boolean notify, XEvent *event) {
    long delta = (long)(page ? number->number_box.page_step : number->number_box.step);
    long next = (long)number->number_box.value + (direction < 0 ? -delta : delta);
    if (next < number->number_box.minimum || next > number->number_box.maximum) {
        if (!number->number_box.wrap) {
            next = next < number->number_box.minimum ? number->number_box.minimum
                                                     : number->number_box.maximum;
        } else if (next < number->number_box.minimum) next = number->number_box.maximum;
        else next = number->number_box.minimum;
    }
    number->number_box.invalid = False;
    if (number->number_box.value != (int)next) {
        number->number_box.value = (int)next;
        format_value(number);
        redraw(number);
        if (notify) notify_value(number, event);
    }
}

static void Events(Widget widget, XtPointer data, XEvent *event,
                   Boolean *dispatch) {
    XmNumberBoxWidget number = (XmNumberBoxWidget)widget;
    char input[8]; KeySym symbol; int length;
    (void)data; (void)dispatch;
    if (event->type == ButtonPress) {
        XButtonEvent *button = &event->xbutton;
        if (button->x >= (int)number->core.width - 24)
            step_value(number, button->y < (int)number->core.height / 2 ? 1 : -1,
                       False, True, event);
        return;
    }
    if (event->type != KeyPress) return;
    length = XLookupString(&event->xkey, input, sizeof(input) - 1, &symbol, NULL);
    if (symbol == XK_Up) { step_value(number, 1, False, True, event); return; }
    if (symbol == XK_Down) { step_value(number, -1, False, True, event); return; }
    if (symbol == XK_Page_Up) { step_value(number, 1, True, True, event); return; }
    if (symbol == XK_Page_Down) { step_value(number, -1, True, True, event); return; }
    if (symbol == XK_Return) { commit_text(number, event); return; }
    if (symbol == XK_Escape) { number->number_box.invalid = False; format_value(number); redraw(number); return; }
    if (symbol == XK_BackSpace || symbol == XK_Delete) {
        size_t length_now = strlen(number->number_box.text);
        if (length_now) number->number_box.text[length_now - 1] = '\0';
    } else if (length == 1 && ((input[0] >= '0' && input[0] <= '9') ||
                               (input[0] == '-' && !number->number_box.text[0]))) {
        size_t length_now = strlen(number->number_box.text);
        if (length_now + 1 < sizeof(number->number_box.text)) {
            number->number_box.text[length_now] = input[0];
            number->number_box.text[length_now + 1] = '\0';
        }
    } else return;
    number->number_box.invalid = False;
    redraw(number);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmNumberBoxWidget old = (XmNumberBoxWidget)old_widget;
    XmNumberBoxWidget number = (XmNumberBoxWidget)widget;
    Boolean redraw_needed = False;
    (void)request; (void)args; (void)count;
    normalize(number);
    if (old->number_box.minimum != number->number_box.minimum ||
        old->number_box.maximum != number->number_box.maximum ||
        old->number_box.value != number->number_box.value ||
        old->number_box.step != number->number_box.step ||
        old->number_box.page_step != number->number_box.page_step ||
        old->number_box.wrap != number->number_box.wrap)
        redraw_needed = True;
    if (old->number_box.value != number->number_box.value) format_value(number);
    return redraw_needed;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    (void)widget; (void)request;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = 140; reply->height = 32;
    return XtGeometryAlmost;
}

void XmNumberBoxGetRange(Widget widget, int *minimum, int *maximum) {
    XmNumberBoxWidget number;
    if (minimum) *minimum = 0;
    if (maximum) *maximum = 0;
    if (!XmIsNumberBox(widget)) return;
    number = (XmNumberBoxWidget)widget;
    if (minimum) *minimum = number->number_box.minimum;
    if (maximum) *maximum = number->number_box.maximum;
}
int XmNumberBoxGetValue(Widget widget) {
    return XmIsNumberBox(widget) ? ((XmNumberBoxWidget)widget)->number_box.value : 0;
}
void XmNumberBoxSetRange(Widget widget, int minimum, int maximum, Boolean notify) {
    XmNumberBoxWidget number;
    int old;
    if (!XmIsNumberBox(widget)) return;
    number = (XmNumberBoxWidget)widget; old = number->number_box.value;
    number->number_box.minimum = minimum; number->number_box.maximum = maximum;
    normalize(number); format_value(number); redraw(number);
    if (notify && old != number->number_box.value) notify_value(number, NULL);
}
void XmNumberBoxSetValue(Widget widget, int value, Boolean notify) {
    XmNumberBoxWidget number;
    int old;
    if (!XmIsNumberBox(widget)) return;
    number = (XmNumberBoxWidget)widget; old = number->number_box.value;
    number->number_box.value = value; normalize(number); format_value(number);
    number->number_box.invalid = False; redraw(number);
    if (notify && old != number->number_box.value) notify_value(number, NULL);
}
void XmNumberBoxStep(Widget widget, int direction, Boolean page, Boolean notify) {
    if (XmIsNumberBox(widget))
        step_value((XmNumberBoxWidget)widget, direction, page, notify, NULL);
}
Boolean XmNumberBoxIsInvalid(Widget widget) {
    return XmIsNumberBox(widget) ? ((XmNumberBoxWidget)widget)->number_box.invalid : False;
}
Widget XmCreateNumberBox(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmNumberBoxWidgetClass, parent, args, count);
}
Widget XmVaCreateNumberBox(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmNumberBoxWidgetClass,
        parent, False, var, count); va_end(var); return widget;
}
Widget XmVaCreateManagedNumberBox(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmNumberBoxWidgetClass,
        parent, True, var, count); va_end(var); return widget;
}
