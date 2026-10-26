#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Xm/SkeletonP.h>
#include <Xm/XmP.h>
#include <Xm/VaSimpleP.h>
#include <X11/Xlib.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *); static void Destroy(Widget); static void Redisplay(Widget, XEvent *, Region); static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *); static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *); static void tick(XtPointer, XtIntervalId *);
#define Offset(field) XtOffsetOf(XmSkeletonRec, skeleton.field)
static XtResource resources[] = {
    {XmNskeletonLabel, XmCSkeletonLabel, XmRXmString, sizeof(XmString), Offset(label), XmRImmediate, NULL},
    {XmNskeletonAnimated, XmCSkeletonAnimated, XmRBoolean, sizeof(Boolean), Offset(animated), XmRImmediate, (XtPointer)True},
    {XmNskeletonReducedMotion, XmCSkeletonReducedMotion, XmRBoolean, sizeof(Boolean), Offset(reduced_motion), XmRImmediate, (XtPointer)False},
    {XmNskeletonInterval, XmCSkeletonInterval, XmRImmediate, sizeof(unsigned long), Offset(interval), XmRImmediate, (XtPointer)120}
};
externaldef(xmskeletonclassrec) XmSkeletonClassRec xmSkeletonClassRec = {
    {.superclass = (WidgetClass)&xmPrimitiveClassRec, .class_name = "XmSkeleton", .widget_size = sizeof(XmSkeletonRec), .initialize = Initialize,
     .realize = XtInheritRealize, .resources = resources, .num_resources = XtNumber(resources), .compress_motion = True, .compress_exposure = XtExposeCompressMultiple,
     .compress_enterleave = True, .destroy = Destroy, .expose = Redisplay, .set_values = SetValues, .set_values_almost = XtInheritSetValuesAlmost,
     .version = XtVersion, .tm_table = XtInheritTranslations, .query_geometry = QueryGeometry},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight, XtInheritTranslations, NULL, NULL, 0, NULL}, {NULL}};
externaldef(xmskeletonwidgetclass) WidgetClass xmSkeletonWidgetClass = (WidgetClass)&xmSkeletonClassRec;
static XmString copy_string(XmString value) { return value ? XmStringCopy(value) : XmStringCreateLocalized(""); }
static void schedule(XmSkeletonWidget skeleton) { if (skeleton->skeleton.running && skeleton->skeleton.interval) skeleton->skeleton.timer = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)skeleton), skeleton->skeleton.interval, tick, skeleton); }
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) { XmSkeletonWidget skeleton = (XmSkeletonWidget)widget; XGCValues values; (void)request; (void)args; (void)count; skeleton->skeleton.label = copy_string(skeleton->skeleton.label); skeleton->skeleton.running = skeleton->skeleton.animated && !skeleton->skeleton.reduced_motion; skeleton->skeleton.phase = 0; values.foreground = skeleton->primitive.bottom_shadow_color; values.background = skeleton->core.background_pixel; skeleton->skeleton.gc = XCreateGC(XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)), GCForeground | GCBackground, &values); if (!skeleton->core.width) skeleton->core.width = 160; if (!skeleton->core.height) skeleton->core.height = 16; schedule(skeleton); }
static void Destroy(Widget widget) { XmSkeletonWidget skeleton = (XmSkeletonWidget)widget; if (skeleton->skeleton.timer) XtRemoveTimeOut(skeleton->skeleton.timer); if (skeleton->skeleton.gc) XFreeGC(XtDisplay(widget), skeleton->skeleton.gc); XmStringFree(skeleton->skeleton.label); }
static void Redisplay(Widget widget, XEvent *event, Region region) { XmSkeletonWidget skeleton = (XmSkeletonWidget)widget; Display *display = XtDisplay(widget); XGCValues values; int stripe_x; (void)event; (void)region; if (!XtWindow(widget) || !skeleton->skeleton.gc) return; values.foreground = skeleton->primitive.bottom_shadow_color; XChangeGC(display, skeleton->skeleton.gc, GCForeground, &values); XFillRectangle(display, XtWindow(widget), skeleton->skeleton.gc, 0, 0, skeleton->core.width, skeleton->core.height); if (!skeleton->skeleton.running || skeleton->core.width == 0) return; values.foreground = skeleton->primitive.top_shadow_color; XChangeGC(display, skeleton->skeleton.gc, GCForeground, &values); stripe_x = (int)(skeleton->skeleton.phase % (skeleton->core.width + 32)); XFillRectangle(display, XtWindow(widget), skeleton->skeleton.gc, stripe_x - 32, 0, 32, skeleton->core.height); }
static Boolean SetValues(Widget old_widget, Widget request, Widget widget, ArgList args, Cardinal *count) { XmSkeletonWidget old = (XmSkeletonWidget)old_widget, skeleton = (XmSkeletonWidget)widget; Boolean changed; (void)request; (void)args; (void)count; if (old->skeleton.label != skeleton->skeleton.label) { XmStringFree(old->skeleton.label); skeleton->skeleton.label = copy_string(skeleton->skeleton.label); } changed = old->skeleton.animated != skeleton->skeleton.animated || old->skeleton.reduced_motion != skeleton->skeleton.reduced_motion || old->skeleton.interval != skeleton->skeleton.interval; if (changed) { if (skeleton->skeleton.timer) XtRemoveTimeOut(skeleton->skeleton.timer); skeleton->skeleton.running = skeleton->skeleton.animated && !skeleton->skeleton.reduced_motion; skeleton->skeleton.timer = (XtIntervalId)0; schedule(skeleton); } return changed || old->skeleton.label != skeleton->skeleton.label; }
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request, XtWidgetGeometry *reply) { (void)request; reply->request_mode = CWWidth | CWHeight; reply->width = widget->core.width ? widget->core.width : 160; reply->height = widget->core.height ? widget->core.height : 16; return XtGeometryYes; }
static void tick(XtPointer data, XtIntervalId *id) { XmSkeletonWidget skeleton = (XmSkeletonWidget)data; (void)id; skeleton->skeleton.timer = (XtIntervalId)0; if (!skeleton->skeleton.running) return; skeleton->skeleton.phase += 8; XClearArea(XtDisplay((Widget)skeleton), XtWindow((Widget)skeleton), 0, 0, 0, 0, True); schedule(skeleton); }
Widget XmCreateSkeleton(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmSkeletonWidgetClass, parent, args, count); }
Widget XmVaCreateSkeleton(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmSkeletonWidgetClass,parent,False,v,n); va_end(v); return w; }
Widget XmVaCreateManagedSkeleton(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmSkeletonWidgetClass,parent,True,v,n); va_end(v); return w; }
void XmSkeletonStart(Widget widget) { XmSkeletonWidget skeleton; if (!widget || !XmIsSkeleton(widget)) return; skeleton = (XmSkeletonWidget)widget; skeleton->skeleton.animated = True; skeleton->skeleton.reduced_motion = False; skeleton->skeleton.running = True; if (!skeleton->skeleton.timer) schedule(skeleton); }
void XmSkeletonStop(Widget widget) { XmSkeletonWidget skeleton; if (!widget || !XmIsSkeleton(widget)) return; skeleton = (XmSkeletonWidget)widget; if (skeleton->skeleton.timer) XtRemoveTimeOut(skeleton->skeleton.timer); skeleton->skeleton.timer = (XtIntervalId)0; skeleton->skeleton.running = False; XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True); }
Boolean XmSkeletonIsAnimating(Widget widget) { return widget && XmIsSkeleton(widget) ? ((XmSkeletonWidget)widget)->skeleton.running : False; }
