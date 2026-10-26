/* Additive compact integer stepper. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <stdint.h>
#include <Xm/StepperP.h>
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
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmStepperWidget stepper = (XmStepperWidget)widget;
    (void)offset;
    if (stepper->stepper.check_set_render_table)
        value->addr = NULL;
    else {
        stepper->stepper.check_set_render_table = True;
        value->addr = (char *)&stepper->stepper.render_table;
    }
}
#define Offset(field) XtOffsetOf(XmStepperRec, field)
static XtResource resources[] = {
    {XmNstepperMinimum, XmCStepperMinimum, XmRInt, sizeof(int), Offset(stepper.minimum), XmRImmediate, (XtPointer)0},
    {XmNstepperMaximum, XmCStepperMaximum, XmRInt, sizeof(int), Offset(stepper.maximum), XmRImmediate, (XtPointer)100},
    {XmNstepperValue, XmCStepperValue, XmRInt, sizeof(int), Offset(stepper.value), XmRImmediate, (XtPointer)0},
    {XmNstepperStep, XmCStepperStep, XmRInt, sizeof(int), Offset(stepper.step), XmRImmediate, (XtPointer)1},
    {XmNstepperWrap, XmCStepperWrap, XmRBoolean, sizeof(Boolean), Offset(stepper.wrap), XmRImmediate, (XtPointer)False},
    {XmNstepperValueChangedCallback, XmCStepperValueChangedCallback, XmRCallback, sizeof(XtCallbackList), Offset(stepper.value_changed_callback), XmRPointer, (XtPointer)NULL},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmRenderTable), Offset(stepper.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable), Offset(stepper.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean), Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension, sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc, (XtPointer)_XmSetThickness}
};
externaldef(xmstepperclassrec) XmStepperClassRec xmStepperClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmStepper", sizeof(XmStepperRec), NULL, NULL, False,
     Initialize, NULL, XtInheritRealize, NULL, 0, resources, XtNumber(resources), NULLQUARK,
     True, XtExposeCompressMaximal, True, False, Destroy, NULL, Redisplay, SetValues, NULL,
     XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL, XtInheritTranslations,
     QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight, XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}
};
externaldef(xmstepperwidgetclass) WidgetClass xmStepperWidgetClass = (WidgetClass)&xmStepperClassRec;

static void normalize(XmStepperWidget s) {
    if (s->stepper.maximum < s->stepper.minimum) s->stepper.maximum = s->stepper.minimum;
    if (s->stepper.value < s->stepper.minimum) s->stepper.value = s->stepper.minimum;
    if (s->stepper.value > s->stepper.maximum) s->stepper.value = s->stepper.maximum;
    if (s->stepper.step < 1) s->stepper.step = 1;
}
static void redraw(XmStepperWidget s) {
    if (XtIsRealized((Widget)s)) XClearArea(XtDisplay((Widget)s), XtWindow((Widget)s), 0, 0, 0, 0, True);
}

static void notify(XmStepperWidget s, XEvent *event) {
    XmStepperCallbackStruct c = {XmCR_VALUE_CHANGED, event, s->stepper.minimum,
                                 s->stepper.maximum, s->stepper.value, s->stepper.step};
    XtCallCallbacks((Widget)s, XmNstepperValueChangedCallback, &c);
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmStepperWidget s = (XmStepperWidget)widget; XGCValues values;
    (void)request; (void)args; (void)count; normalize(s);
    if (!s->core.width) s->core.width = 72;
    if (!s->core.height) s->core.height = 32;
    values.foreground = s->primitive.foreground; values.background = s->core.background_pixel;
    if (s->stepper.render_table == NULL)
        s->stepper.render_table = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        s->stepper.render_table = XmFontListCopy(s->stepper.render_table);
    s->stepper.gc = XCreateGC(XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
                              GCForeground | GCBackground, &values);
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events, NULL);
}
static void Destroy(Widget widget) {
    XmStepperWidget s = (XmStepperWidget)widget;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events, NULL);
    if (s->stepper.gc) XFreeGC(XtDisplay(widget), s->stepper.gc);
    if (s->stepper.render_table)
        XmRenderTableFree(s->stepper.render_table);
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmStepperWidget s = (XmStepperWidget)widget; Display *d = XtDisplay(widget); Window w = XtWindow(widget);
    char text[32]; Position split;
    (void)event; (void)region;
    if (!w || !s->stepper.gc) return;
    XClearWindow(d, w);
    snprintf(text, sizeof(text), "%d", s->stepper.value);
    if (s->stepper.render_table) {
        XmString value = XmStringCreateLocalized(text);
        Dimension text_width = 0, text_height = 0;
        if (value) {
            XmStringExtent(s->stepper.render_table, value,
                           &text_width, &text_height);
            XmStringDraw(d, w, s->stepper.render_table, value, s->stepper.gc,
                         8, ((int)s->core.height - (int)text_height) / 2,
                         text_width, XmALIGNMENT_BEGINNING,
                         XmPrim_layout_direction(s), NULL);
            XmStringFree(value);
        }
    }
    split = s->core.width > 28 ? s->core.width - 28 : 0;
    XDrawLine(d, w, s->stepper.gc, split, 0, split, s->core.height);
    XDrawLine(d, w, s->stepper.gc, split + 14, 5, split + 14, s->core.height - 5);
    if (s->stepper.render_table) {
        const char *symbols[2] = {"+", "−"};
        int centers[2] = {split + 7, split + 21};
        int cell_height = (int)s->core.height / 2;
        int i;
        for (i = 0; i < 2; ++i) {
            XmString symbol = XmStringCreateLocalized((char *)symbols[i]);
            Dimension symbol_width = 0, symbol_height = 0;
            if (symbol) {
                XmStringExtent(s->stepper.render_table, symbol,
                               &symbol_width, &symbol_height);
                XmStringDraw(d, w, s->stepper.render_table, symbol,
                             s->stepper.gc,
                             centers[i] - (int)symbol_width / 2,
                             i * cell_height +
                               (cell_height - (int)symbol_height) / 2,
                             symbol_width, XmALIGNMENT_BEGINNING,
                             XmPrim_layout_direction(s), NULL);
                XmStringFree(symbol);
            }
        }
    }
    if (s->primitive.highlighted && s->primitive.highlight_thickness)
        XDrawRectangle(d, w, s->stepper.gc, s->primitive.highlight_thickness - 1,
                       s->primitive.highlight_thickness - 1,
                       s->core.width - 2 * s->primitive.highlight_thickness,
                       s->core.height - 2 * s->primitive.highlight_thickness);
}
static void step(XmStepperWidget s, int direction, Boolean notify_change, XEvent *event) {
    int64_t next = (int64_t)s->stepper.value +
                   (direction < 0 ? -(int64_t)s->stepper.step :
                                    (int64_t)s->stepper.step);
    if (next < s->stepper.minimum || next > s->stepper.maximum) {
        if (s->stepper.wrap) next = next < s->stepper.minimum ? s->stepper.maximum : s->stepper.minimum;
        else next = next < s->stepper.minimum ? s->stepper.minimum : s->stepper.maximum;
    }
    if (s->stepper.value != (int)next) { s->stepper.value = (int)next; redraw(s); if (notify_change) notify(s, event); }
}
static void Events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) {
    XmStepperWidget s = (XmStepperWidget)widget; KeySym key; char buf[4]; int len;
    (void)data; (void)dispatch;
    if (event->type == ButtonPress) { step(s, event->xbutton.x < (int)s->core.width - 28 ? 1 : (event->xbutton.y < (int)s->core.height / 2 ? 1 : -1), True, event); return; }
    if (event->type != KeyPress) return;
    len = XLookupString(&event->xkey, buf, sizeof(buf), &key, NULL);
    (void)len;
    if (key == XK_Up || key == XK_Right) step(s, 1, True, event);
    else if (key == XK_Down || key == XK_Left) step(s, -1, True, event);
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmStepperWidget old = (XmStepperWidget)old_widget, s = (XmStepperWidget)widget; Boolean redraw_needed = False;
    (void)request; (void)args; (void)count; normalize(s);
    if (old->stepper.render_table != s->stepper.render_table) {
        XmRenderTable new_table = s->stepper.render_table ?
            XmFontListCopy(s->stepper.render_table) :
            XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (new_table) {
            if (old->stepper.render_table)
                XmRenderTableFree(old->stepper.render_table);
            s->stepper.render_table = new_table;
            redraw_needed = True;
        } else {
            /* Keep the last valid table if a replacement cannot be copied. */
            s->stepper.render_table = old->stepper.render_table;
        }
    }
    if (old->stepper.minimum != s->stepper.minimum || old->stepper.maximum != s->stepper.maximum || old->stepper.value != s->stepper.value || old->stepper.step != s->stepper.step || old->stepper.wrap != s->stepper.wrap) redraw_needed = True;
    return redraw_needed;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request, XtWidgetGeometry *reply) {
    (void)widget; (void)request; reply->request_mode = CWWidth | CWHeight; reply->width = 72; reply->height = 32; return XtGeometryAlmost;
}
void XmStepperGetRange(Widget widget, int *minimum, int *maximum) { XmStepperWidget s; if (minimum) *minimum = 0; if (maximum) *maximum = 0; if (widget == NULL || !XmIsStepper(widget)) return; s = (XmStepperWidget)widget; if (minimum) *minimum = s->stepper.minimum; if (maximum) *maximum = s->stepper.maximum; }
int XmStepperGetValue(Widget widget) { return widget != NULL && XmIsStepper(widget) ? ((XmStepperWidget)widget)->stepper.value : 0; }
void XmStepperSetRange(Widget widget, int minimum, int maximum, Boolean notify_change) { XmStepperWidget s; int old; if (widget == NULL || !XmIsStepper(widget)) return; s = (XmStepperWidget)widget; old = s->stepper.value; s->stepper.minimum = minimum; s->stepper.maximum = maximum; normalize(s); redraw(s); if (notify_change && old != s->stepper.value) notify(s, NULL); }
void XmStepperSetValue(Widget widget, int value, Boolean notify_change) { XmStepperWidget s; int old; if (widget == NULL || !XmIsStepper(widget)) return; s = (XmStepperWidget)widget; old = s->stepper.value; s->stepper.value = value; normalize(s); redraw(s); if (notify_change && old != s->stepper.value) notify(s, NULL); }
void XmStepperStep(Widget widget, int direction, Boolean notify_change) { if (widget != NULL && XmIsStepper(widget)) step((XmStepperWidget)widget, direction, notify_change, NULL); }
Widget XmCreateStepper(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmStepperWidgetClass, parent, args, count); }
Widget XmVaCreateStepper(Widget parent, char *name, ...) { Widget widget; va_list var; int count; Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var); Va_start(var, name); widget = XmeVLCreateWidget(name, xmStepperWidgetClass, parent, False, var, count); va_end(var); return widget; }
Widget XmVaCreateManagedStepper(Widget parent, char *name, ...) { Widget widget; va_list var; int count; Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var); Va_start(var, name); widget = XmeVLCreateWidget(name, xmStepperWidgetClass, parent, True, var, count); va_end(var); return widget; }
