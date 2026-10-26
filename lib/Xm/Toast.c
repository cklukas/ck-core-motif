#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Xm/ToastP.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <X11/keysym.h>
#include <limits.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *); static void Destroy(Widget);
static void Resize(Widget); static void ChangeManaged(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void activate(Widget, XtPointer, XtPointer); static void events(Widget, XtPointer, XEvent *, Boolean *);
static void timeout(XtPointer, XtIntervalId *);
#define Offset(field) XtOffsetOf(XmToastRec, toast.field)
static XtResource resources[] = {
    {XmNtoastMessage, XmCToastMessage, XmRXmString, sizeof(XmString), Offset(message), XmRImmediate, NULL},
    {XmNtoastActionLabel, XmCToastActionLabel, XmRXmString, sizeof(XmString), Offset(action_label), XmRImmediate, NULL},
    {XmNtoastSeverity, XmCToastSeverity, XmRUnsignedChar, sizeof(unsigned char), Offset(severity), XmRImmediate, (XtPointer)XmTOAST_SEVERITY_INFO},
    {XmNtoastTimeout, XmCToastTimeout, XmRImmediate, sizeof(unsigned long), Offset(timeout), XmRImmediate, (XtPointer)5000},
    {XmNtoastActionCallback, XmCToastActionCallback, XmRCallback, sizeof(XtCallbackList), Offset(action_callback), XmRPointer, NULL},
    {XmNtoastDismissCallback, XmCToastDismissCallback, XmRCallback, sizeof(XtCallbackList), Offset(dismiss_callback), XmRPointer, NULL}
};
externaldef(xmtoastclassrec) XmToastClassRec xmToastClassRec = {
    {.superclass = (WidgetClass)&xmManagerClassRec, .class_name = "XmToast", .widget_size = sizeof(XmToastRec),
     .initialize = Initialize, .realize = XtInheritRealize, .resources = resources, .num_resources = XtNumber(resources),
     .compress_motion = True, .compress_exposure = XtExposeCompressMultiple, .compress_enterleave = True,
     .destroy = Destroy, .resize = Resize, .set_values = SetValues, .set_values_almost = XtInheritSetValuesAlmost,
     .version = XtVersion, .tm_table = XtInheritTranslations, .query_geometry = QueryGeometry},
    {.geometry_manager = GeometryManager, .change_managed = ChangeManaged, .insert_child = XtInheritInsertChild,
     .delete_child = XtInheritDeleteChild}, {.constraint_size = sizeof(XmManagerConstraintRec)},
    {.translations = XtInheritTranslations, .parent_process = XmInheritParentProcess}, {NULL}};
externaldef(xmtoastwidgetclass) WidgetClass xmToastWidgetClass = (WidgetClass)&xmToastClassRec;
static XmString copy_string(XmString source) { return source ? XmStringCopy(source) : XmStringCreateLocalized(""); }
static Boolean has_action(XmToastWidget toast) { return toast->toast.action_label && !XmStringEmpty(toast->toast.action_label); }
static Dimension clamp_dimension(unsigned long value) { return (Dimension)(value > USHRT_MAX ? USHRT_MAX : value ? value : 1); }
static void preferred(Widget child, Dimension *width, Dimension *height) { XtWidgetGeometry wanted; wanted.request_mode = 0; XtQueryGeometry(child, NULL, &wanted); *width = wanted.request_mode & CWWidth ? wanted.width : child->core.width; *height = wanted.request_mode & CWHeight ? wanted.height : child->core.height; }
static void natural(XmToastWidget toast, Dimension *width, Dimension *height) { Dimension mw, mh, aw = 0, ah = 0; preferred(toast->toast.message_widget, &mw, &mh); if (has_action(toast)) preferred(toast->toast.action_widget, &aw, &ah); *width = clamp_dimension((unsigned long)mw + (has_action(toast) ? aw + 12 : 0) + 16); *height = clamp_dimension((unsigned long)(mh > ah ? mh : ah) + 16); }
static void layout(XmToastWidget toast) { Dimension mw, mh, aw = 0, ah = 0, action = has_action(toast) ? 1 : 0, width; if (toast->toast.initializing || toast->toast.laying_out) return; toast->toast.laying_out = True; preferred(toast->toast.message_widget, &mw, &mh); if (action) preferred(toast->toast.action_widget, &aw, &ah); width = clamp_dimension(toast->core.width > 16 ? toast->core.width - 16 : 1); XtSetMappedWhenManaged(toast->toast.action_widget, action); XmeConfigureObject(toast->toast.message_widget, 8, 8, action && width > aw + 12 ? width - aw - 12 : width, mh, 0); if (action) XmeConfigureObject(toast->toast.action_widget, (Position)(toast->core.width > aw + 8 ? toast->core.width - aw - 8 : 8), 8, aw, ah, 0); toast->toast.laying_out = False; }
static void negotiate(XmToastWidget toast) { Dimension width, height, rw, rh; XtGeometryResult result; if (toast->toast.initializing || toast->toast.laying_out) return; natural(toast, &width, &height); result = XtMakeResizeRequest((Widget)toast, width, height, &rw, &rh); if (result == XtGeometryAlmost) XtMakeResizeRequest((Widget)toast, rw, rh, NULL, NULL); layout(toast); }
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) { XmToastWidget toast = (XmToastWidget)widget; Dimension width, height; (void)request; (void)args; (void)count; toast->toast.initializing = True; toast->toast.timer = (XtIntervalId)0; toast->toast.message = copy_string(toast->toast.message); toast->toast.action_label = copy_string(toast->toast.action_label); toast->toast.message_widget = XtVaCreateManagedWidget("message", xmLabelWidgetClass, widget, XmNlabelString, toast->toast.message, XmNborderWidth, 0, NULL); toast->toast.action_widget = XtVaCreateManagedWidget("action", xmPushButtonWidgetClass, widget, XmNlabelString, toast->toast.action_label, XmNborderWidth, 0, XmNmappedWhenManaged, has_action(toast), NULL); XtAddCallback(toast->toast.action_widget, XmNactivateCallback, activate, toast); XtAddEventHandler(widget, KeyPressMask, False, events, NULL); toast->toast.initializing = False; natural(toast, &width, &height); if (!toast->core.width) toast->core.width = width; if (!toast->core.height) toast->core.height = height; layout(toast); }
static void Destroy(Widget widget) { XmToastWidget toast = (XmToastWidget)widget; if (toast->toast.timer) XtRemoveTimeOut(toast->toast.timer); XtRemoveEventHandler(widget, KeyPressMask, False, events, NULL); XmStringFree(toast->toast.message); XmStringFree(toast->toast.action_label); }
static void Resize(Widget widget) { layout((XmToastWidget)widget); } static void ChangeManaged(Widget widget) { negotiate((XmToastWidget)widget); }
static Boolean SetValues(Widget old_widget, Widget request, Widget widget, ArgList args, Cardinal *count) { XmToastWidget old = (XmToastWidget)old_widget, toast = (XmToastWidget)widget; Boolean changed = old->toast.message != toast->toast.message || old->toast.action_label != toast->toast.action_label || old->toast.timeout != toast->toast.timeout; (void)request; (void)args; (void)count; if (old->toast.message != toast->toast.message) { XmStringFree(old->toast.message); toast->toast.message = copy_string(toast->toast.message); } if (old->toast.action_label != toast->toast.action_label) { XmStringFree(old->toast.action_label); toast->toast.action_label = copy_string(toast->toast.action_label); } XtVaSetValues(toast->toast.message_widget, XmNlabelString, toast->toast.message, NULL); XtVaSetValues(toast->toast.action_widget, XmNlabelString, toast->toast.action_label, XmNmappedWhenManaged, has_action(toast), NULL); if (changed) negotiate(toast); return changed; }
static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request, XtWidgetGeometry *reply) { XmToastWidget toast = (XmToastWidget)XtParent(child); if (request->request_mode & (CWX | CWY | CWBorderWidth | CWSibling | CWStackMode)) return XtGeometryNo; if (!toast->toast.laying_out && !(request->request_mode & XtCWQueryOnly)) negotiate(toast); reply->request_mode = CWWidth | CWHeight; reply->width = child->core.width; reply->height = child->core.height; return XtGeometryAlmost; }
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *intended, XtWidgetGeometry *reply) { XmToastWidget toast = (XmToastWidget)widget; natural(toast, &reply->width, &reply->height); reply->request_mode = CWWidth | CWHeight; if (intended && intended->width == reply->width && intended->height == reply->height) return XtGeometryYes; return toast->core.width == reply->width && toast->core.height == reply->height ? XtGeometryNo : XtGeometryAlmost; }
static void activate(Widget widget, XtPointer client, XtPointer call) { XmToastWidget toast = (XmToastWidget)client; XmToastCallbackStruct callback; (void)widget; callback.reason = XmCR_ACTIVATE; callback.event = call ? ((XmPushButtonCallbackStruct *)call)->event : NULL; callback.severity = toast->toast.severity; XtCallCallbacks((Widget)toast, XmNtoastActionCallback, &callback); }
static void events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) { KeySym key; char input[2]; (void)data; (void)dispatch; if (event->type == KeyPress) { XLookupString(&event->xkey, input, sizeof(input), &key, NULL); if (key == XK_Escape) XmToastDismiss(widget, event); } }
static void timeout(XtPointer data, XtIntervalId *id) { XmToastWidget toast = (XmToastWidget)data; (void)id; toast->toast.timer = (XtIntervalId)0; XmToastDismiss((Widget)toast, NULL); }
Widget XmCreateToast(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmToastWidgetClass, parent, args, count); }
Widget XmVaCreateToast(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmToastWidgetClass,parent,False,v,n); va_end(v); return w; }
Widget XmVaCreateManagedToast(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmToastWidgetClass,parent,True,v,n); va_end(v); return w; }
void XmToastShow(Widget widget) { XmToastWidget toast; if (!widget || !XmIsToast(widget)) return; toast = (XmToastWidget)widget; if (toast->toast.timer) XtRemoveTimeOut(toast->toast.timer); toast->toast.shown = True; XtManageChild(widget); if (!toast->toast.paused && toast->toast.timeout) toast->toast.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), toast->toast.timeout, timeout, toast); }
void XmToastDismiss(Widget widget, XEvent *event) { XmToastWidget toast; XmToastCallbackStruct callback; if (!widget || !XmIsToast(widget)) return; toast = (XmToastWidget)widget; if (toast->toast.timer) { XtRemoveTimeOut(toast->toast.timer); toast->toast.timer = (XtIntervalId)0; } if (!toast->toast.shown) return; toast->toast.shown = False; XtUnmanageChild(widget); callback.reason = XmCR_UNMAP; callback.event = event; callback.severity = toast->toast.severity; XtCallCallbacks(widget, XmNtoastDismissCallback, &callback); }
void XmToastPause(Widget widget, Boolean paused) { XmToastWidget toast; if (!widget || !XmIsToast(widget)) return; toast = (XmToastWidget)widget; toast->toast.paused = paused; if (paused && toast->toast.timer) { XtRemoveTimeOut(toast->toast.timer); toast->toast.timer = (XtIntervalId)0; } else if (!paused && toast->toast.shown && toast->toast.timeout && !toast->toast.timer) toast->toast.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), toast->toast.timeout, timeout, toast); }
void XmToastInvokeAction(Widget widget, XEvent *event) { XmToastWidget toast; XmToastCallbackStruct callback; if (!widget || !XmIsToast(widget) || !has_action((XmToastWidget)widget)) return; toast = (XmToastWidget)widget; callback.reason = XmCR_ACTIVATE; callback.event = event; callback.severity = toast->toast.severity; XtCallCallbacks(widget, XmNtoastActionCallback, &callback); }
Boolean XmToastIsShown(Widget widget) { return widget && XmIsToast(widget) ? ((XmToastWidget)widget)->toast.shown : False; }
