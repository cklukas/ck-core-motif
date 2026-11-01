#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <limits.h>
#include <Xm/RatingControlP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "CallbackI.h"
#include "XmI.h"
#include "AppearanceDrawI.h"
#include <Xm/Appearance.h>
#include <limits.h>
#include <string.h>
static void Initialize(Widget, Widget, ArgList, Cardinal *); static void Redisplay(Widget, XEvent *, Region); static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *); static void Destroy(Widget); static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *); static void Events(Widget, XtPointer, XEvent *, Boolean *);
static void Highlight(Widget);
static void Unhighlight(Widget);
#define RATING_STAR_POINTS 10
#define RATING_EVENTS (KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask)
#define Offset(field) XtOffsetOf(XmRatingControlRec, field)
static XtResource resources[] = {
 {XmNratingCount, XmCRatingCount, XmRInt, sizeof(int), Offset(rating.count), XmRImmediate, (XtPointer)5},
 {XmNratingValue, XmCRatingValue, XmRInt, sizeof(int), Offset(rating.value), XmRImmediate, (XtPointer)0},
 {XmNratingAllowClear, XmCRatingAllowClear, XmRBoolean, sizeof(Boolean), Offset(rating.allow_clear), XmRImmediate, (XtPointer)True},
 {XmNratingReadonly, XmCRatingReadonly, XmRBoolean, sizeof(Boolean), Offset(rating.readonly), XmRImmediate, (XtPointer)False},
 {XmNratingValueChangedCallback, XmCRatingValueChangedCallback, XmRCallback, sizeof(XtCallbackList), Offset(rating.value_changed_callback), XmRPointer, (XtPointer)NULL},
 {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean), Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
 {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension, sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc, (XtPointer)_XmSetThickness}
};
externaldef(xmratingcontrolclassrec) XmRatingControlClassRec xmRatingControlClassRec = {{(WidgetClass)&xmPrimitiveClassRec,"XmRatingControl",sizeof(XmRatingControlRec),NULL,NULL,False,Initialize,NULL,XtInheritRealize,NULL,0,resources,XtNumber(resources),NULLQUARK,True,XtExposeCompressMaximal,True,False,Destroy,NULL,Redisplay,SetValues,NULL,XtInheritSetValuesAlmost,NULL,NULL,XtVersion,NULL,(String)_XmPrimitiveTabFocusTranslations,QueryGeometry,NULL,NULL},{Highlight,Unhighlight,XtInheritTranslations,NULL,NULL,0,NULL},{NULL}};
externaldef(xmratingcontrolwidgetclass) WidgetClass xmRatingControlWidgetClass=(WidgetClass)&xmRatingControlClassRec;
static void normalize(XmRatingControlWidget rating) { if (rating->rating.count < 1) rating->rating.count=1; if (rating->rating.value<0) rating->rating.value=0; if (rating->rating.value>rating->rating.count) rating->rating.value=rating->rating.count; }
static void draw_star(Display *display, Window window, GC gc, int center_x,
                      int center_y, int radius, Boolean filled) {
    static const int x_shape[RATING_STAR_POINTS] = {0, 3, 10, 5, 6, 0, -6, -5, -10, -3};
    static const int y_shape[RATING_STAR_POINTS] = {-10, -3, -3, 2, 8, 5, 8, 2, -3, -3};
    XPoint points[RATING_STAR_POINTS];
    int i;
    for (i = 0; i < RATING_STAR_POINTS; ++i) {
        int x = center_x + x_shape[i] * radius / 10;
        int y = center_y + y_shape[i] * radius / 10;
        points[i].x = (short)(x < SHRT_MIN ? SHRT_MIN :
                              x > SHRT_MAX ? SHRT_MAX : x);
        points[i].y = (short)(y < SHRT_MIN ? SHRT_MIN :
                              y > SHRT_MAX ? SHRT_MAX : y);
    }
    if (filled)
        XFillPolygon(display, window, gc, points, RATING_STAR_POINTS,
                     Complex, CoordModeOrigin);
    XDrawLines(display, window, gc, points, RATING_STAR_POINTS,
                   CoordModeOrigin);
    XDrawLine(display, window, gc, points[RATING_STAR_POINTS - 1].x,
              points[RATING_STAR_POINTS - 1].y, points[0].x, points[0].y);
}
static void notify(XmRatingControlWidget rating, XEvent *event) {
    if (!rating || rating->core.being_destroyed) return;
    XmRatingControlCallbackStruct c = {XmCR_VALUE_CHANGED, event, rating->rating.count, rating->rating.value};
    (void)_XmCallCallbacksUntilDestroyed((Widget)rating, XmNratingValueChangedCallback, &c);
}
static void redraw(Widget widget) {
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
}
typedef struct {
    Dimension width, height, inset, cell, diameter;
    Boolean rtl;
} RatingMetrics;

static void metrics(XmRatingControlWidget rating, RatingMetrics *m) {
    Widget w = (Widget)rating;
    XmAppearanceProfile profile = _XmAppearanceGeometryProfile(XtDisplay(w));
    Boolean compact = (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK) &&
        _XmAppearanceGeometryDensity(XtDisplay(w)) == XmAPPEARANCE_DENSITY_COMPACT;
    unsigned long frame = (unsigned long)rating->primitive.highlight_thickness +
                          rating->primitive.shadow_thickness;
    m->inset = (Dimension)MIN(USHRT_MAX, MAX(frame, (unsigned long)_XmDpiDefaultDimension(w, 4)));
    m->cell = _XmDpiDefaultDimension(w, compact ? 28 : 32);
    m->diameter = _XmDpiDefaultDimension(w, compact ? 18 : 22);
    m->width = (Dimension)MIN((unsigned long long)USHRT_MAX,
        2ULL * m->inset + (unsigned long long)rating->rating.count * m->cell);
    m->height = (Dimension)MIN((unsigned long)USHRT_MAX,
        2UL * m->inset + _XmDpiDefaultDimension(w, compact ? 20 : 26));
    m->rtl = XmDirectionMatchPartial(XmPrim_layout_direction(rating), XmRIGHT_TO_LEFT, XmHORIZONTAL_MASK);
}

static void natural_size(XmRatingControlWidget rating) {
    RatingMetrics m; metrics(rating, &m);
    if (rating->rating.automatic_width) rating->core.width = m.width;
    if (rating->rating.automatic_height) rating->core.height = m.height;
}

static void ScaleChanged(Widget widget, double old_scale, double new_scale, XtPointer closure) {
    (void)closure;
    if (old_scale != new_scale && !widget->core.being_destroyed)
        XtSetValues(widget, NULL, 0);
}

static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmRatingControlWidget rating = (XmRatingControlWidget)widget;
    XGCValues values;
    (void)args; (void)count;
    normalize(rating);
    rating->rating.automatic_width = request->core.width == 0;
    rating->rating.automatic_height = request->core.height == 0;
    natural_size(rating);
    rating->rating.armed = False;
    rating->rating.hover_value = 0;
    values.foreground = rating->primitive.foreground;
    values.background = rating->core.background_pixel;
    rating->rating.gc = XCreateGC(XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
                                  GCForeground | GCBackground, &values);
    XtAddEventHandler(widget, RATING_EVENTS, False, Events, NULL);
    (void)_XmRegisterScaleChange(widget, ScaleChanged, NULL);
}

static void Destroy(Widget widget) {
    XmRatingControlWidget rating = (XmRatingControlWidget)widget;
    _XmUnregisterScaleChange(widget, ScaleChanged, NULL);
    XtRemoveEventHandler(widget, RATING_EVENTS, False, Events, NULL);
    if (rating->rating.gc) XFreeGC(XtDisplay(widget), rating->rating.gc);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmRatingControlWidget rating = (XmRatingControlWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    RatingMetrics m;
    int width, height;
    int shown = XtIsSensitive(widget) && !rating->rating.readonly && rating->rating.hover_value ?
        rating->rating.hover_value : rating->rating.value;
    (void)event; (void)region;
    if (!window || !rating->rating.gc) return;
    metrics(rating, &m);
    XClearWindow(display, window);
    width = (int)rating->core.width - 2 * m.inset;
    height = (int)rating->core.height - 2 * m.inset;
    /* Bound work by available pixels, including an arbitrarily large count. */
    if (width > 0 && height > 0 && rating->rating.count <= width) {
        XRectangle clip = {m.inset, m.inset, width, height};
        XSetClipRectangles(display, rating->rating.gc, 0, 0, &clip, 1, Unsorted);
        for (int i = 0; i < rating->rating.count; ++i) {
            int index = m.rtl ? rating->rating.count - 1 - i : i;
            int left = (int)((long long)index * width / rating->rating.count);
            int right = (int)((long long)(index + 1) * width / rating->rating.count);
            int diameter = MIN(m.diameter, MIN(right - left - 2, height - 2));
            if (diameter < 2) continue;
            XSetForeground(display, rating->rating.gc,
                !XtIsSensitive(widget) ? rating->primitive.bottom_shadow_color :
                i < shown ? rating->primitive.highlight_color : rating->primitive.foreground);
            draw_star(display, window, rating->rating.gc,
                      m.inset + (left + right - 1) / 2,
                      m.inset + (height - 1) / 2, (diameter - 1) / 2,
                      i < shown);
        }
        XSetClipMask(display, rating->rating.gc, None);
    }
    if (!_XmAppearanceDrawButtonFocus(widget, rating->primitive.highlighted) &&
        rating->primitive.highlighted &&
        rating->core.width > 2 * rating->primitive.highlight_thickness &&
        rating->core.height > 2 * rating->primitive.highlight_thickness)
        xmPrimitiveClassRec.primitive_class.border_highlight(widget);
}
static void Highlight(Widget widget) {
    XmRatingControlWidget rating = (XmRatingControlWidget)widget;
    rating->primitive.highlighted = True;
    rating->primitive.highlight_drawn = True;
    Redisplay(widget, NULL, NULL);
}
static void Unhighlight(Widget widget) {
    XmRatingControlWidget rating = (XmRatingControlWidget)widget;
    rating->primitive.highlighted = False;
    rating->primitive.highlight_drawn = False;
    Redisplay(widget, NULL, NULL);
}

static int hit_value(XmRatingControlWidget rating, int x, int y) {
    RatingMetrics m; metrics(rating, &m);
    int width = (int)rating->core.width - 2 * m.inset;
    int height = (int)rating->core.height - 2 * m.inset;
    x -= m.inset; y -= m.inset;
    if (width <= 0 || height <= 0 || rating->rating.count > width ||
        x < 0 || x >= width || y < 0 || y >= height) return 0;
    if (m.rtl) x = width - 1 - x;
    return (int)(((long long)(x + 1) * rating->rating.count - 1) / width) + 1;
}

static void choose(XmRatingControlWidget rating, int value, Boolean toggle_clear, XEvent *event) {
    Widget widget = (Widget)rating;
    if (!XtIsSensitive(widget) || rating->rating.readonly || widget->core.being_destroyed) return;
    value = MAX(0, MIN(rating->rating.count, value));
    if (toggle_clear && value == rating->rating.value && rating->rating.allow_clear) value = 0;
    if (value == rating->rating.value) return;
    rating->rating.value = value;
    redraw(widget);
    notify(rating, event);
}

static void Events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) {
    XmRatingControlWidget rating = (XmRatingControlWidget)widget;
    int value;
    (void)data; (void)dispatch;
    if (!widget || !event || !XmIsRatingControl(widget) || !XtIsSensitive(widget) || rating->rating.readonly || widget->core.being_destroyed) return;
    switch (event->type) {
    case EnterNotify:
    case MotionNotify:
        value = event->type == MotionNotify ? hit_value(rating, event->xmotion.x, event->xmotion.y) :
                                             hit_value(rating, event->xcrossing.x, event->xcrossing.y);
        if (value != rating->rating.hover_value) { rating->rating.hover_value = value; redraw(widget); }
        return;
    case LeaveNotify:
        rating->rating.hover_value = 0; redraw(widget); return;
    case ButtonPress:
        if (event->xbutton.button != Button1) return;
        value = hit_value(rating, event->xbutton.x, event->xbutton.y);
        if (!value) return;
        rating->rating.armed = True; rating->rating.hover_value = value;
        redraw(widget);
        XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
        return;
    case ButtonRelease:
        if (event->xbutton.button != Button1) return;
        value = hit_value(rating, event->xbutton.x, event->xbutton.y);
        { Boolean armed = rating->rating.armed;
          rating->rating.armed = False; rating->rating.hover_value = 0; redraw(widget);
          if (armed && value) choose(rating, value, True, event);
        }
        return;
    case KeyPress:
        { KeySym key = XLookupKeysym(&event->xkey, 0);
          RatingMetrics m; metrics(rating, &m);
          int delta = 0;
          if (dispatch && (key == XK_Left || key == XK_Right || key == XK_Up || key == XK_Down ||
              key == XK_Home || key == XK_End ||
              (rating->rating.allow_clear && (key == XK_Delete || key == XK_BackSpace))))
              *dispatch = False;
          rating->rating.hover_value = 0; rating->rating.armed = False; redraw(widget);
          if (key == XK_Left) delta = m.rtl ? 1 : -1;
          else if (key == XK_Right) delta = m.rtl ? -1 : 1;
          else if (key == XK_Up) delta = 1;
          else if (key == XK_Down) delta = -1;
          if (delta) {
              value = rating->rating.value;
              if (delta > 0 && value < rating->rating.count) ++value;
              if (delta < 0 && value > 0) --value;
              choose(rating, value, False, event);
          } else if (key == XK_Home) choose(rating, 0, False, event);
          else if (key == XK_End) choose(rating, rating->rating.count, False, event);
          else if ((key == XK_Delete || key == XK_BackSpace) && rating->rating.allow_clear)
              choose(rating, 0, False, event);
        }
        return;
    default: return;
    }
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmRatingControlWidget rating = (XmRatingControlWidget)widget;
    (void)old_widget; (void)request;
    normalize(rating);
    rating->rating.armed = False;
    rating->rating.hover_value = 0;
    for (Cardinal i = 0; i < *count; ++i) {
        if (!strcmp(args[i].name, XmNwidth)) rating->rating.automatic_width = (Dimension)args[i].value == 0;
        if (!strcmp(args[i].name, XmNheight)) rating->rating.automatic_height = (Dimension)args[i].value == 0;
    }
    natural_size(rating);
    return True;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmRatingControlWidget rating = (XmRatingControlWidget)widget;
    RatingMetrics m; metrics(rating, &m);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = rating->rating.automatic_width ? m.width : rating->core.width;
    reply->height = rating->rating.automatic_height ? m.height : rating->core.height;
    return XmeReplyToQueryGeometry(widget, request, reply);
}
int XmRatingControlGetValue(Widget widget){return widget != NULL && XmIsRatingControl(widget)?((XmRatingControlWidget)widget)->rating.value:0;}
void XmRatingControlSetValue(Widget widget,int value,Boolean notify_change){XmRatingControlWidget rating;int old;if(widget == NULL || !XmIsRatingControl(widget) || widget->core.being_destroyed)return;rating=(XmRatingControlWidget)widget;old=rating->rating.value;rating->rating.value=value;normalize(rating);if(XtIsRealized(widget))XClearArea(XtDisplay(widget),XtWindow(widget),0,0,0,0,True);if(notify_change && old != rating->rating.value)notify(rating,NULL);}
Widget XmCreateRatingControl(Widget parent,char*name,ArgList args,Cardinal count){return XtCreateWidget(name,xmRatingControlWidgetClass,parent,args,count);}
