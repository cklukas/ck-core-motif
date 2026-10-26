#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/RatingControlP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"
static void Initialize(Widget, Widget, ArgList, Cardinal *); static void Redisplay(Widget, XEvent *, Region); static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *); static void Destroy(Widget); static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *); static void Events(Widget, XtPointer, XEvent *, Boolean *);
#define RATING_STAR_POINTS 10
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
externaldef(xmratingcontrolclassrec) XmRatingControlClassRec xmRatingControlClassRec = {{(WidgetClass)&xmPrimitiveClassRec,"XmRatingControl",sizeof(XmRatingControlRec),NULL,NULL,False,Initialize,NULL,XtInheritRealize,NULL,0,resources,XtNumber(resources),NULLQUARK,True,XtExposeCompressMaximal,True,False,Destroy,NULL,Redisplay,SetValues,NULL,XtInheritSetValuesAlmost,NULL,NULL,XtVersion,NULL,XtInheritTranslations,QueryGeometry,NULL,NULL},{XmInheritBorderHighlight,XmInheritBorderUnhighlight,XtInheritTranslations,NULL,NULL,0,NULL},{NULL}};
externaldef(xmratingcontrolwidgetclass) WidgetClass xmRatingControlWidgetClass=(WidgetClass)&xmRatingControlClassRec;
static void normalize(XmRatingControlWidget rating) { if (rating->rating.count < 1) rating->rating.count=1; if (rating->rating.value<0) rating->rating.value=0; if (rating->rating.value>rating->rating.count) rating->rating.value=rating->rating.count; }
static void draw_star(Display *display, Window window, GC gc, int center_x,
                      int center_y, int radius, Boolean filled) {
    static const int x_shape[RATING_STAR_POINTS] = {0, 3, 10, 5, 6, 0, -6, -5, -10, -3};
    static const int y_shape[RATING_STAR_POINTS] = {-10, -3, -3, 2, 8, 5, 8, 2, -3, -3};
    XPoint points[RATING_STAR_POINTS];
    int i;
    for (i = 0; i < RATING_STAR_POINTS; ++i) {
        points[i].x = (short)(center_x + x_shape[i] * radius / 10);
        points[i].y = (short)(center_y + y_shape[i] * radius / 10);
    }
    if (filled)
        XFillPolygon(display, window, gc, points, RATING_STAR_POINTS,
                     Convex, CoordModeOrigin);
    else
        XDrawLines(display, window, gc, points, RATING_STAR_POINTS,
                   CoordModeOrigin);
    XDrawLine(display, window, gc, points[RATING_STAR_POINTS - 1].x,
              points[RATING_STAR_POINTS - 1].y, points[0].x, points[0].y);
}
static void notify(XmRatingControlWidget rating, XEvent *event) { XmRatingControlCallbackStruct c={XmCR_VALUE_CHANGED,event,rating->rating.count,rating->rating.value}; XtCallCallbacks((Widget)rating,XmNratingValueChangedCallback,&c); }
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) { XmRatingControlWidget rating=(XmRatingControlWidget)widget; XGCValues values; (void)request;(void)args;(void)count; normalize(rating); if(!rating->core.width)rating->core.width=180; if(!rating->core.height)rating->core.height=32; values.foreground=rating->primitive.foreground; values.background=rating->core.background_pixel; rating->rating.gc=XCreateGC(XtDisplay(widget),DefaultRootWindow(XtDisplay(widget)),GCForeground|GCBackground,&values); XtAddEventHandler(widget,KeyPressMask|ButtonPressMask,False,Events,NULL); }
static void Destroy(Widget widget) { XmRatingControlWidget rating=(XmRatingControlWidget)widget; XtRemoveEventHandler(widget,KeyPressMask|ButtonPressMask,False,Events,NULL); if(rating->rating.gc)XFreeGC(XtDisplay(widget),rating->rating.gc); }
static void Redisplay(Widget widget, XEvent *event, Region region)
{
    XmRatingControlWidget rating = (XmRatingControlWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    const Dimension gap = 4;
    Dimension segment, segment_width, segment_height;
    int i;

    (void)event;
    (void)region;
    if (!window || !rating->rating.gc)
        return;
    XClearWindow(display, window);
    segment = rating->core.width / (Dimension)rating->rating.count;
    if (!segment)
        return;
    segment_width = segment > 2 * gap ? segment - 2 * gap : segment;
    segment_height = rating->core.height > 12 ? rating->core.height - 12 :
                     rating->core.height;
    for (i = 0; i < rating->rating.count; i++) {
        int x = (int)(i * segment + (segment > 2 * gap ? gap : 0));
        int center_x = x + (int)segment_width / 2;
        int center_y = segment_height == rating->core.height ?
                       (int)segment_height / 2 : 6 + (int)segment_height / 2;
        int radius = ((int)segment_width < (int)segment_height ?
                      (int)segment_width : (int)segment_height) / 2 - 1;
        if (radius < 5) radius = 5;
        XSetForeground(display, rating->rating.gc,
                       rating->rating.value > i ? rating->primitive.highlight_color :
                                                   rating->primitive.foreground);
        draw_star(display, window, rating->rating.gc, center_x, center_y,
                  radius, rating->rating.value > i);
    }
    if (rating->primitive.highlighted && rating->primitive.highlight_thickness &&
        rating->core.width > 2 * rating->primitive.highlight_thickness &&
        rating->core.height > 2 * rating->primitive.highlight_thickness)
        XDrawRectangle(display, window, rating->rating.gc,
                       rating->primitive.highlight_thickness - 1,
                       rating->primitive.highlight_thickness - 1,
                       rating->core.width - 2 * rating->primitive.highlight_thickness,
                       rating->core.height - 2 * rating->primitive.highlight_thickness);
}
static void choose(XmRatingControlWidget rating,int value,Boolean notify_change,XEvent*event){int old;if(rating->rating.readonly)return;old=rating->rating.value;if(value==old&&rating->rating.allow_clear)value=0;if(value<0)value=0;if(value>rating->rating.count)value=rating->rating.count;if(old==value)return;rating->rating.value=value;if(XtIsRealized((Widget)rating))XClearArea(XtDisplay((Widget)rating),XtWindow((Widget)rating),0,0,0,0,True);if(notify_change)notify(rating,event);}
static void Events(Widget widget,XtPointer data,XEvent*event,Boolean*dispatch){XmRatingControlWidget rating=(XmRatingControlWidget)widget;KeySym key;char buf[2];int len; (void)data;(void)dispatch;if(event->type==ButtonPress){if (rating->core.width == 0) return; long long scaled=(long long)event->xbutton.x*rating->rating.count; int value=(int)(scaled/(int)rating->core.width)+1;choose(rating,value,True,event);return;}if(event->type!=KeyPress)return;len=XLookupString(&event->xkey,buf,sizeof(buf),&key,NULL);(void)len;if(key==XK_Left||key==XK_Down)choose(rating,rating->rating.value-1,True,event);else if(key==XK_Right||key==XK_Up)choose(rating,rating->rating.value+1,True,event);else if(key==XK_Delete||key==XK_BackSpace)choose(rating,0,True,event);}
static Boolean SetValues(Widget old_widget,Widget request,Widget widget,ArgList args,Cardinal*count){XmRatingControlWidget old=(XmRatingControlWidget)old_widget,rating=(XmRatingControlWidget)widget;Boolean redraw=False;(void)request;(void)args;(void)count;normalize(rating);if(old->rating.count!=rating->rating.count||old->rating.value!=rating->rating.value||old->rating.readonly!=rating->rating.readonly)redraw=True;return redraw;}
static XtGeometryResult QueryGeometry(Widget widget,XtWidgetGeometry*request,XtWidgetGeometry*reply){(void)widget;(void)request;reply->request_mode=CWWidth|CWHeight;reply->width=180;reply->height=32;return XtGeometryAlmost;}
int XmRatingControlGetValue(Widget widget){return widget != NULL && XmIsRatingControl(widget)?((XmRatingControlWidget)widget)->rating.value:0;}
void XmRatingControlSetValue(Widget widget,int value,Boolean notify_change){XmRatingControlWidget rating;if(widget == NULL || !XmIsRatingControl(widget))return;rating=(XmRatingControlWidget)widget;rating->rating.value=value;normalize(rating);if(XtIsRealized(widget))XClearArea(XtDisplay(widget),XtWindow(widget),0,0,0,0,True);if(notify_change)notify(rating,NULL);}
Widget XmCreateRatingControl(Widget parent,char*name,ArgList args,Cardinal count){return XtCreateWidget(name,xmRatingControlWidgetClass,parent,args,count);}
