/* Additive packed-RGBA picker; XmColorSelector remains unchanged. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/ColorPickerP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);
#define Offset(field) XtOffsetOf(XmColorPickerRec, field)
static XtResource resources[] = {
    {XmNcolorPickerValue, XmCColorPickerValue, XmRCardinal,
     sizeof(Cardinal), Offset(color_picker.value), XmRImmediate,
     (XtPointer)0x336699ffUL},
    {XmNcolorPickerPreviewCallback, XmCColorPickerPreviewCallback,
     XmRCallback, sizeof(XtCallbackList), Offset(color_picker.preview_callback),
     XmRPointer, (XtPointer)NULL},
    {XmNcolorPickerValueChangedCallback, XmCColorPickerValueChangedCallback,
     XmRCallback, sizeof(XtCallbackList), Offset(color_picker.value_changed_callback),
     XmRPointer, (XtPointer)NULL},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};
externaldef(xmcolorpickerclassrec) XmColorPickerClassRec xmColorPickerClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmColorPicker", sizeof(XmColorPickerRec),
     NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0, resources,
     XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True, False,
     Destroy, NULL, Redisplay, SetValues, NULL, XtInheritSetValuesAlmost, NULL,
     NULL, XtVersion, NULL, XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight, XtInheritTranslations,
     NULL, NULL, 0, NULL}, {NULL}
};
externaldef(xmcolorpickerwidgetclass) WidgetClass xmColorPickerWidgetClass =
    (WidgetClass)&xmColorPickerClassRec;

static void notify(XmColorPickerWidget picker, String resource, XEvent *event,
                   unsigned long value) {
    XmColorPickerCallbackStruct callback = {XmCR_VALUE_CHANGED, event, value};
    XtCallCallbacks((Widget)picker, resource, &callback);
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmColorPickerWidget picker = (XmColorPickerWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
    picker->color_picker.preview = picker->color_picker.value;
    values.foreground = picker->primitive.foreground;
    values.background = picker->core.background_pixel;
    picker->color_picker.gc = XCreateGC(XtDisplay(widget),
        DefaultRootWindow(XtDisplay(widget)), GCForeground | GCBackground, &values);
    if (!picker->core.width) picker->core.width = 180;
    if (!picker->core.height) picker->core.height = 34;
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events, NULL);
}
static void Destroy(Widget widget) {
    XmColorPickerWidget picker = (XmColorPickerWidget)widget;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events, NULL);
    if (picker->color_picker.gc) XFreeGC(XtDisplay(widget), picker->color_picker.gc);
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmColorPickerWidget picker = (XmColorPickerWidget)widget;
    Display *display = XtDisplay(widget); Window window = XtWindow(widget);
    char text[16]; int baseline;
    unsigned long value = picker->color_picker.preview;
    XGCValues values; GC swatch; XColor color;
    (void)event; (void)region;
    if (!window || !picker->color_picker.gc) return;
    XClearWindow(display, window);
    color.red = (unsigned short)(((value >> 24) & 0xff) * 257);
    color.green = (unsigned short)(((value >> 16) & 0xff) * 257);
    color.blue = (unsigned short)(((value >> 8) & 0xff) * 257);
    color.flags = DoRed | DoGreen | DoBlue;
    if (!XAllocColor(display, DefaultColormapOfScreen(XtScreen(widget)), &color))
        color.pixel = picker->primitive.foreground;
    values.foreground = color.pixel;
    swatch = XCreateGC(display, window, GCForeground, &values);
    if (swatch) {
        Dimension swatch_height = picker->core.height > 10 ?
                                  picker->core.height - 10 : 0;
        if (swatch_height)
            XFillRectangle(display, window, swatch, 5, 5, 24, swatch_height);
        XFreeGC(display, swatch);
    }
    snprintf(text, sizeof(text), "#%08lx", value & 0xffffffffUL);
    baseline = (int)(picker->core.height / 2);
    XDrawString(display, window, picker->color_picker.gc, 38, baseline, text,
                (int)strlen(text));
    if (picker->primitive.highlighted && picker->primitive.highlight_thickness &&
        picker->core.width > 2 * picker->primitive.highlight_thickness &&
        picker->core.height > 2 * picker->primitive.highlight_thickness)
        XDrawRectangle(display, window, picker->color_picker.gc,
                       picker->primitive.highlight_thickness - 1,
                       picker->primitive.highlight_thickness - 1,
                       picker->core.width - 2 * picker->primitive.highlight_thickness,
                       picker->core.height - 2 * picker->primitive.highlight_thickness);
}
static void Events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) {
    XmColorPickerWidget picker = (XmColorPickerWidget)widget;
    KeySym key; char input[8]; int length; unsigned long value;
    (void)data; (void)dispatch;
    if (event->type == ButtonPress) {
        XmColorPickerPreviewValue(widget, picker->color_picker.preview ^ 0x000000ffUL);
        return;
    }
    if (event->type != KeyPress) return;
    length = XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_Return || key == XK_KP_Enter) {
        XmColorPickerSetValue(widget, picker->color_picker.preview, True);
        return;
    }
    if (key == XK_Escape) {
        picker->color_picker.preview = picker->color_picker.value;
        if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
        return;
    }
    if (length != 1) return;
    value = picker->color_picker.preview;
    if (input[0] == 'r' || input[0] == 'R') value ^= 0x01000000UL;
    else if (input[0] == 'g' || input[0] == 'G') value ^= 0x00010000UL;
    else if (input[0] == 'b' || input[0] == 'B') value ^= 0x00000100UL;
    else return;
    XmColorPickerPreviewValue(widget, value);
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmColorPickerWidget old = (XmColorPickerWidget)old_widget;
    XmColorPickerWidget picker = (XmColorPickerWidget)widget;
    (void)request; (void)args; (void)count;
    if (old->color_picker.value != picker->color_picker.value) {
        picker->color_picker.preview = picker->color_picker.value;
        return True;
    }
    return False;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    (void)widget; (void)request; reply->request_mode = CWWidth | CWHeight;
    reply->width = 180; reply->height = 34; return XtGeometryAlmost;
}
unsigned long XmColorPickerGetValue(Widget widget) {
    return XmIsColorPicker(widget) ? ((XmColorPickerWidget)widget)->color_picker.value : 0;
}
void XmColorPickerSetValue(Widget widget, unsigned long value, Boolean notify_change) {
    XmColorPickerWidget picker; if (!XmIsColorPicker(widget)) return;
    picker = (XmColorPickerWidget)widget; picker->color_picker.value = value;
    picker->color_picker.preview = value;
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
    if (notify_change) notify(picker, XmNcolorPickerValueChangedCallback, NULL, value);
}
void XmColorPickerPreviewValue(Widget widget, unsigned long value) {
    XmColorPickerWidget picker; if (!XmIsColorPicker(widget)) return;
    picker = (XmColorPickerWidget)widget; picker->color_picker.preview = value;
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
    notify(picker, XmNcolorPickerPreviewCallback, NULL, value);
}
Widget XmCreateColorPicker(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmColorPickerWidgetClass, parent, args, count);
}
