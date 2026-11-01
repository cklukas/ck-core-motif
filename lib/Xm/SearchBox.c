#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/SearchBoxP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <string.h>
#include <stdlib.h>
#include "CallbackI.h"
#include "PrimitiveI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void DecorationInsets(Widget, Dimension *, Dimension *);
static void DrawDecoration(Widget, const XRectangle *);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);
static void query_changed(Widget, XtPointer, XtPointer);
static void committed(Widget, XtPointer, XtPointer);
#define Offset(field) XtOffsetOf(XmSearchBoxRec, field)
static XtResource resources[] = {
    {XmNsearchPlaceholder, XmCSearchPlaceholder, XmRXmString, sizeof(XmString), Offset(search.placeholder), XmRImmediate, NULL},
    {XmNsearchClearButton, XmCSearchClearButton, XmRBoolean, sizeof(Boolean), Offset(search.clear_button), XmRImmediate, (XtPointer)True},
    {XmNsearchQueryChangedCallback, XmCSearchQueryChangedCallback, XmRCallback, sizeof(XtCallbackList), Offset(search.query_changed_callback), XmRPointer, NULL},
    {XmNsearchCommittedCallback, XmCSearchCommittedCallback, XmRCallback, sizeof(XtCallbackList), Offset(search.committed_callback), XmRPointer, NULL},
    {XmNsearchClearedCallback, XmCSearchClearedCallback, XmRCallback, sizeof(XtCallbackList), Offset(search.cleared_callback), XmRPointer, NULL}
};
externaldef(xmsearchboxclassrec) XmSearchBoxClassRec xmSearchBoxClassRec = {
    {(WidgetClass)&xmTextFieldClassRec, "XmSearchBox", sizeof(XmSearchBoxRec), NULL,
     NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0, resources,
     XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True, False,
     Destroy, XtInheritResize, XtInheritExpose, SetValues, NULL, XtInheritSetValuesAlmost, NULL,
     NULL, XtVersion, NULL, XtInheritTranslations, XtInheritQueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight, XtInheritTranslations,
     NULL, NULL, 0, NULL}, {NULL, DecorationInsets, DrawDecoration}, {NULL}};
externaldef(xmsearchboxwidgetclass) WidgetClass xmSearchBoxWidgetClass =
    (WidgetClass)&xmSearchBoxClassRec;

static XmString copy_label(XmString source) {
    return source ? XmStringCopy(source) : XmStringCreateLocalized("");
}
static char *query_copy(Widget widget) {
    char *value = XmTextFieldGetString(widget);
    return value ? value : XtNewString("");
}
static Boolean emit(Widget widget, String resource, int reason, XEvent *event) {
    Boolean alive;
    XmSearchBoxCallbackStruct callback;
    if (!widget || !XmIsSearchBox(widget) || widget->core.being_destroyed) return False;
    char *query = query_copy(widget);
    if (!query) return True;
    callback.reason = reason; callback.event = event; callback.query = query;
    alive = _XmCallCallbacksUntilDestroyed(widget, resource, &callback);
    XtFree(query);
    return alive;
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget;
    (void)request; (void)args; (void)count;
    box->search.placeholder = copy_label(box->search.placeholder);
    box->search.clearing = False;
    box->search.clear_armed = False;
    XtAddCallback(widget, XmNvalueChangedCallback, query_changed, NULL);
    XtAddCallback(widget, XmNactivateCallback, committed, NULL);
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask | ButtonReleaseMask | LeaveWindowMask, False, Events, NULL);
}
static void Destroy(Widget widget) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget;
    XtRemoveCallback(widget, XmNvalueChangedCallback, query_changed, NULL);
    XtRemoveCallback(widget, XmNactivateCallback, committed, NULL);
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask | ButtonReleaseMask | LeaveWindowMask, False, Events, NULL);
    if (box->search.placeholder)
        XmStringFree(box->search.placeholder);
}
/* Icons are measured from the field font. The editor keeps its ordinary
 * margin resource; TextField reserves these additional decoration lanes. */
static unsigned icon_size(XmSearchBoxWidget box) {
    unsigned size = box->text.font_ascent + box->text.font_descent;
    if (size < 8) size = 8;
    if (size > 128) size = 128;
    return size;
}
static Boolean right_to_left(XmSearchBoxWidget box) {
    return XmDirectionMatch(box->primitive.layout_direction, XmRIGHT_TO_LEFT);
}
static void DecorationInsets(Widget widget, Dimension *left, Dimension *right) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget;
    unsigned size = icon_size(box), lane = size + (size + 3) / 4;
    *left = (Dimension)lane;
    *right = box->search.clear_button ? (Dimension)lane : 0;
    if (right_to_left(box)) { Dimension swap = *left; *left = *right; *right = swap; }
}
static Boolean icon_rect(XmSearchBoxWidget box, Boolean clear, XRectangle *rect) {
    unsigned size = icon_size(box);
    unsigned border = box->primitive.shadow_thickness + box->primitive.highlight_thickness;
    unsigned margin = box->text.margin_width;
    Dimension left, right;
    Boolean on_right = clear != right_to_left(box);
    DecorationInsets((Widget)box, &left, &right);
    if (2 * (border + margin) + left + right >= box->core.width ||
        2 * border + size > box->core.height) return False;
    rect->x = (short)(on_right ? box->core.width - border - margin - size : border + margin);
    rect->y = (short)((box->core.height - size) / 2);
    rect->width = rect->height = (unsigned short)size;
    return True;
}
static void DrawDecoration(Widget widget, const XRectangle *text_rect) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget;
    Display *display = XtDisplay(widget);
    XGCValues values;
    XRectangle rect;
    GC gc;
    unsigned size = icon_size(box), stroke = (size + 7) / 8;
    values.foreground = XtIsSensitive(widget) ? box->primitive.foreground :
                                              box->primitive.bottom_shadow_color;
    values.background = box->core.background_pixel;
    gc = XCreateGC(display, XtWindow(widget), GCForeground | GCBackground, &values);
    if (!gc) return;
    for (unsigned clear = 0; clear < 2; ++clear) {
        if (clear && !box->search.clear_button) continue;
        if (!icon_rect(box, clear, &rect)) continue;
        XSetForeground(display, gc, box->core.background_pixel);
        XFillRectangle(display, XtWindow(widget), gc, rect.x, rect.y, rect.width, rect.height);
        XSetForeground(display, gc, values.foreground);
        if (clear && (!box->text.string_length || !box->text.editable)) continue;
        /* Pixel-centred masks make circle and diagonal strokes deterministic
         * on every X server, without partial-arc endpoint conventions. */
        for (unsigned y = 0; y < size; ++y) for (unsigned x = 0; x < size; ++x) {
            Boolean ink;
            if (clear) {
                int d1 = (int)x - (int)y, d2 = (int)x + (int)y - (int)size + 1;
                unsigned pad = size / 4;
                ink = x >= pad && y >= pad && x < size - pad && y < size - pad &&
                      (abs(d1) < (int)stroke || abs(d2) < (int)stroke);
            } else {
                int diameter = (int)(size * 3 / 4), cx = diameter - 1;
                int dx = 2 * (int)x - cx, dy = 2 * (int)y - cx;
                int radius = diameter - 2, inner = radius - 2 * (int)stroke;
                int distance = dx * dx + dy * dy;
                ink = distance <= radius * radius && distance >= inner * inner;
                if (x >= size / 2 && y >= size / 2 &&
                    abs((int)x - (int)y) < (int)stroke) ink = True;
            }
            if (ink) XDrawPoint(display, XtWindow(widget), gc, rect.x + x, rect.y + y);
        }
    }
    if (!box->text.string_length && box->search.placeholder && text_rect->width && text_rect->height) {
        XSetClipRectangles(display, gc, 0, 0, (XRectangle *)text_rect, 1, Unsorted);
        XSetForeground(display, gc, box->primitive.bottom_shadow_color);
        XmStringDraw(display, XtWindow(widget), box->text.font_list, box->search.placeholder,
                     gc, text_rect->x, text_rect->y, text_rect->width,
                     XmALIGNMENT_BEGINNING, right_to_left(box) ? XmSTRING_DIRECTION_R_TO_L :
                                                              XmSTRING_DIRECTION_L_TO_R,
                     (XRectangle *)text_rect);
    }
    XFreeGC(display, gc);
}
static Boolean clear_hit(XmSearchBoxWidget box, int x, int y) {
    XRectangle rect;
    unsigned border = box->primitive.shadow_thickness + box->primitive.highlight_thickness;
    if (!box->text.editable || !box->search.clear_button || !box->text.string_length || !icon_rect(box, True, &rect))
        return False;
    return x >= rect.x && x < rect.x + rect.width &&
           y >= (int)border && y < (int)box->core.height - (int)border;
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmSearchBoxWidget old = (XmSearchBoxWidget)old_widget, box = (XmSearchBoxWidget)widget;
    XmString replacement;
    (void)request; (void)args; (void)count;
    if (old->search.placeholder != box->search.placeholder) {
        replacement = copy_label(box->search.placeholder);
        if (replacement) {
            XmStringFree(old->search.placeholder);
            box->search.placeholder = replacement;
        } else {
            box->search.placeholder = old->search.placeholder;
        }
    }
    if (!XtIsSensitive(widget) || !box->text.editable || !box->search.clear_button)
        box->search.clear_armed = False;
    return old->text.editable != box->text.editable ||
           old->search.placeholder != box->search.placeholder ||
           old->search.clear_button != box->search.clear_button;
}
static void query_changed(Widget widget, XtPointer closure, XtPointer data) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget;
    (void)closure; (void)data;
    if (box == NULL || !XmIsSearchBox(widget) || box->core.being_destroyed) return;
    if (!box->search.clearing) emit(widget, XmNsearchQueryChangedCallback,
                                    XmSEARCH_REASON_QUERY_CHANGED, NULL);
}
static void committed(Widget widget, XtPointer closure, XtPointer data) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget;
    if (box == NULL || !XmIsSearchBox(widget) || box->core.being_destroyed) return;
    (void)closure; (void)data;
    emit(widget, XmNsearchCommittedCallback, XmSEARCH_REASON_COMMITTED, NULL);
}
static void Events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget; KeySym key; char input[8];
    (void)data;
    if (box == NULL || event == NULL || !XmIsSearchBox(widget) || box->core.being_destroyed) return;
    if (!XtIsSensitive(widget)) { box->search.clear_armed = False; return; }
    if (event->type == LeaveNotify) { box->search.clear_armed = False; return; }
    if (event->type == ButtonPress && event->xbutton.button == Button1 &&
        clear_hit(box, event->xbutton.x, event->xbutton.y)) {
        if (!_XmPrimitiveFocusOnPrimaryClick(widget, event)) return;
        box->search.clear_armed = True;
        if (dispatch) *dispatch = False;
        return;
    }
    if (event->type == ButtonRelease && event->xbutton.button == Button1 && box->search.clear_armed) {
        Boolean activate = clear_hit(box, event->xbutton.x, event->xbutton.y);
        box->search.clear_armed = False;
        if (dispatch) *dispatch = False;
        if (activate) XmSearchBoxClear(widget);
        return;
    }
    if (event->type != KeyPress) return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_Escape && box->search.clear_button && box->text.editable) {
        if (dispatch) *dispatch = False;
        XmSearchBoxClear(widget);
    }
}
char *XmSearchBoxGetString(Widget widget) {
    return widget != NULL && XmIsSearchBox(widget) ? XmTextFieldGetString(widget) : NULL;
}
void XmSearchBoxSetString(Widget widget, const char *value) {
    if (widget != NULL && XmIsSearchBox(widget) &&
        !((XmSearchBoxWidget)widget)->core.being_destroyed)
        XmTextFieldSetString(widget, (char *)(value ? value : ""));
}
static void clear_destroyed(Widget widget, XtPointer closure, XtPointer data) {
    (void)widget; (void)data;
    *(Boolean *)closure = True;
}
void XmSearchBoxClear(Widget widget) {
    XmSearchBoxWidget box;
    Boolean destroyed = False;
    char *value;
    if (widget == NULL || !XmIsSearchBox(widget) || !XtIsSensitive(widget) ||
        widget->core.being_destroyed) return;
    box = (XmSearchBoxWidget)widget;
    if (box->search.clearing) return;
    box->search.clearing = True;
    XtAddCallback(widget, XmNdestroyCallback, clear_destroyed, &destroyed);
    XmTextFieldSetString(widget, (char *)"");
    if (destroyed) return;
    XtRemoveCallback(widget, XmNdestroyCallback, clear_destroyed, &destroyed);
    if (widget->core.being_destroyed) return;
    box->search.clearing = False;
    /* TextField verification can reject or replace a clear request. */
    value = query_copy(widget);
    if (!value || *value) { XtFree(value); return; }
    XtFree(value);
    if (!emit(widget, XmNsearchClearedCallback, XmSEARCH_REASON_CLEARED, NULL))
        return;
    emit(widget, XmNsearchQueryChangedCallback, XmSEARCH_REASON_QUERY_CHANGED, NULL);
}
void XmSearchBoxCommit(Widget widget, XEvent *event) {
    if (widget == NULL || !XmIsSearchBox(widget) || !XtIsSensitive(widget) ||
        widget->core.being_destroyed) return;
    emit(widget, XmNsearchCommittedCallback, XmSEARCH_REASON_COMMITTED, event);
}
Widget XmCreateSearchBox(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmSearchBoxWidgetClass, parent, args, count); }
Widget XmVaCreateSearchBox(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmSearchBoxWidgetClass,parent,False,v,n); va_end(v); return w; }
Widget XmVaCreateManagedSearchBox(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmSearchBoxWidgetClass,parent,True,v,n); va_end(v); return w; }
