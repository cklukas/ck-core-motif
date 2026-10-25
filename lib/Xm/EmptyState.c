#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Xm/EmptyStateP.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Resize(Widget);
static void ChangeManaged(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void activate(Widget, XtPointer, XtPointer);
#define Offset(field) XtOffsetOf(XmEmptyStateRec, empty_state.field)
static XtResource resources[] = {
    {XmNemptyStateIcon, XmCEmptyStateIcon, XmRPixmap, sizeof(Pixmap), Offset(icon), XmRImmediate, (XtPointer)XmUNSPECIFIED_PIXMAP},
    {XmNemptyStateMessage, XmCEmptyStateMessage, XmRXmString, sizeof(XmString), Offset(message), XmRImmediate, NULL},
    {XmNemptyStateActionLabel, XmCEmptyStateActionLabel, XmRXmString, sizeof(XmString), Offset(action_label), XmRImmediate, NULL},
    {XmNemptyStateActionCallback, XmCEmptyStateActionCallback, XmRCallback, sizeof(XtCallbackList), Offset(action_callback), XmRPointer, NULL},
    {XmNemptyStateMarginWidth, XmCEmptyStateMarginWidth, XmRHorizontalDimension, sizeof(Dimension), Offset(margin_width), XmRImmediate, (XtPointer)8},
    {XmNemptyStateMarginHeight, XmCEmptyStateMarginHeight, XmRVerticalDimension, sizeof(Dimension), Offset(margin_height), XmRImmediate, (XtPointer)8},
    {XmNemptyStateSpacing, XmCEmptyStateSpacing, XmRVerticalDimension, sizeof(Dimension), Offset(spacing), XmRImmediate, (XtPointer)6}
};
static XmSyntheticResource synthetic_resources[] = {
    {XmNemptyStateMarginWidth, sizeof(Dimension), Offset(margin_width), XmeFromHorizontalPixels, XmeToHorizontalPixels},
    {XmNemptyStateMarginHeight, sizeof(Dimension), Offset(margin_height), XmeFromVerticalPixels, XmeToVerticalPixels},
    {XmNemptyStateSpacing, sizeof(Dimension), Offset(spacing), XmeFromVerticalPixels, XmeToVerticalPixels}
};
externaldef(xmemptystateclassrec) XmEmptyStateClassRec xmEmptyStateClassRec = {
    {.superclass = (WidgetClass)&xmManagerClassRec, .class_name = "XmEmptyState",
     .widget_size = sizeof(XmEmptyStateRec), .initialize = Initialize,
     .realize = XtInheritRealize, .resources = resources,
     .num_resources = XtNumber(resources), .compress_motion = True,
     .compress_exposure = XtExposeCompressMultiple, .compress_enterleave = True,
     .destroy = Destroy, .resize = Resize, .set_values = SetValues,
     .set_values_almost = XtInheritSetValuesAlmost, .version = XtVersion,
     .tm_table = XtInheritTranslations, .query_geometry = QueryGeometry},
    {.geometry_manager = GeometryManager, .change_managed = ChangeManaged,
     .insert_child = XtInheritInsertChild, .delete_child = XtInheritDeleteChild},
    {.constraint_size = sizeof(XmManagerConstraintRec)},
    {.translations = XtInheritTranslations, .syn_resources = synthetic_resources,
     .num_syn_resources = XtNumber(synthetic_resources),
     .parent_process = XmInheritParentProcess}, {NULL}};
externaldef(xmemptystatewidgetclass) WidgetClass xmEmptyStateWidgetClass =
    (WidgetClass)&xmEmptyStateClassRec;

static XmString copy_string(XmString source) {
    char *text; XmString result;
    if (!source) return XmStringCreateLocalized("");
    text = XmStringUnparse(source, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                           NULL, 0, XmOUTPUT_ALL);
    if (!text) return XmStringCreateLocalized("");
    result = XmStringCreateLocalized(text); XtFree(text); return result;
}
static Dimension dimension(unsigned long value) {
    return (Dimension)(value > USHRT_MAX ? USHRT_MAX : value ? value : 1);
}
static Boolean has_icon(XmEmptyStateWidget state) {
    return state->empty_state.icon != XmUNSPECIFIED_PIXMAP;
}
static Boolean has_action(XmEmptyStateWidget state) {
    char *text;
    Boolean result;
    if (!state->empty_state.action_label) return False;
    text = XmStringUnparse(state->empty_state.action_label, NULL, XmCHARSET_TEXT,
                           XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    result = text && *text;
    XtFree(text);
    return result;
}
static void preferred(Widget child, Dimension *width, Dimension *height) {
    XtWidgetGeometry wanted; wanted.request_mode = 0; XtQueryGeometry(child, NULL, &wanted);
    *width = wanted.request_mode & CWWidth ? wanted.width : child->core.width;
    *height = wanted.request_mode & CWHeight ? wanted.height : child->core.height;
}
static void natural(XmEmptyStateWidget state, Dimension *width, Dimension *height) {
    Dimension iw = 0, ih = 0, mw, mh, aw = 0, ah = 0, max_width = 0, rows = 1;
    if (has_icon(state)) preferred(state->empty_state.icon_widget, &iw, &ih);
    preferred(state->empty_state.message_widget, &mw, &mh);
    if (has_action(state)) preferred(state->empty_state.action_widget, &aw, &ah);
    max_width = iw > mw ? iw : mw; if (aw > max_width) max_width = aw;
    if (has_icon(state)) rows++;
    if (has_action(state)) rows++;
    *width = dimension(2UL * state->empty_state.margin_width + max_width);
    *height = dimension(2UL * state->empty_state.margin_height + ih + mh +
                         (rows - 1) * state->empty_state.spacing + ah);
}
static void layout(XmEmptyStateWidget state) {
    Dimension iw = 0, ih = 0, mw, mh, aw = 0, ah = 0, width;
    Position y; Boolean icon = has_icon(state), action = has_action(state);
    if (state->empty_state.initializing || state->empty_state.laying_out) return;
    state->empty_state.laying_out = True;
    if (icon) preferred(state->empty_state.icon_widget, &iw, &ih);
    preferred(state->empty_state.message_widget, &mw, &mh);
    if (action) preferred(state->empty_state.action_widget, &aw, &ah);
    width = dimension(state->core.width > 2UL * state->empty_state.margin_width
                          ? state->core.width - 2UL * state->empty_state.margin_width : 1);
    y = (Position)state->empty_state.margin_height;
    XtSetMappedWhenManaged(state->empty_state.icon_widget, icon);
    XtSetMappedWhenManaged(state->empty_state.action_widget, action);
    if (icon) { XmeConfigureObject(state->empty_state.icon_widget, state->empty_state.margin_width, y, width, ih, 0); y += ih + state->empty_state.spacing; }
    XmeConfigureObject(state->empty_state.message_widget, state->empty_state.margin_width, y, width, mh, 0);
    y += mh;
    if (action) { y += state->empty_state.spacing; XmeConfigureObject(state->empty_state.action_widget, state->empty_state.margin_width, y, width, ah, 0); }
    state->empty_state.laying_out = False;
}
static void negotiate(XmEmptyStateWidget state) {
    Dimension width, height, rw, rh; XtGeometryResult result;
    if (state->empty_state.initializing || state->empty_state.laying_out) return;
    natural(state, &width, &height); result = XtMakeResizeRequest((Widget)state, width, height, &rw, &rh);
    if (result == XtGeometryAlmost) XtMakeResizeRequest((Widget)state, rw, rh, NULL, NULL);
    layout(state);
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmEmptyStateWidget state = (XmEmptyStateWidget)widget; Dimension width, height;
    (void)request; (void)args; (void)count; state->empty_state.initializing = True; state->empty_state.laying_out = False;
    state->empty_state.message = copy_string(state->empty_state.message);
    state->empty_state.action_label = copy_string(state->empty_state.action_label);
    state->empty_state.icon_widget = XtVaCreateManagedWidget("icon", xmLabelWidgetClass, widget,
        XmNlabelType, XmPIXMAP, XmNlabelPixmap, state->empty_state.icon,
        XmNborderWidth, 0, NULL);
    state->empty_state.message_widget = XtVaCreateManagedWidget("message", xmLabelWidgetClass, widget,
        XmNlabelString, state->empty_state.message, XmNalignment, XmALIGNMENT_CENTER,
        XmNborderWidth, 0, NULL);
    state->empty_state.action_widget = XtVaCreateManagedWidget("action", xmPushButtonWidgetClass, widget,
        XmNlabelString, state->empty_state.action_label, XmNborderWidth, 0,
        XmNmappedWhenManaged, has_action(state), NULL);
    XtAddCallback(state->empty_state.action_widget, XmNactivateCallback, activate, state);
    state->empty_state.initializing = False; natural(state, &width, &height);
    if (!state->core.width) state->core.width = width;
    if (!state->core.height) state->core.height = height;
    layout(state);
}
static void Destroy(Widget widget) {
    XmEmptyStateWidget state = (XmEmptyStateWidget)widget;
    XmStringFree(state->empty_state.message); XmStringFree(state->empty_state.action_label);
}
static void Resize(Widget widget) { layout((XmEmptyStateWidget)widget); }
static void ChangeManaged(Widget widget) { negotiate((XmEmptyStateWidget)widget); }
static Boolean SetValues(Widget old_widget, Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmEmptyStateWidget old = (XmEmptyStateWidget)old_widget, state = (XmEmptyStateWidget)widget;
    Boolean changed = old->empty_state.message != state->empty_state.message ||
                      old->empty_state.action_label != state->empty_state.action_label ||
                      old->empty_state.icon != state->empty_state.icon;
    (void)request; (void)args; (void)count;
    if (old->empty_state.message != state->empty_state.message) { XmStringFree(old->empty_state.message); state->empty_state.message = copy_string(state->empty_state.message); }
    if (old->empty_state.action_label != state->empty_state.action_label) { XmStringFree(old->empty_state.action_label); state->empty_state.action_label = copy_string(state->empty_state.action_label); }
    if (old->empty_state.icon != state->empty_state.icon) XtVaSetValues(state->empty_state.icon_widget, XmNlabelPixmap, state->empty_state.icon, NULL);
    XtVaSetValues(state->empty_state.message_widget, XmNlabelString, state->empty_state.message, NULL);
    XtVaSetValues(state->empty_state.action_widget, XmNlabelString, state->empty_state.action_label, XmNmappedWhenManaged, has_action(state), NULL);
    if (changed || old->empty_state.margin_width != state->empty_state.margin_width || old->empty_state.margin_height != state->empty_state.margin_height || old->empty_state.spacing != state->empty_state.spacing) negotiate(state);
    return False;
}
static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request, XtWidgetGeometry *reply) {
    XmEmptyStateWidget state = (XmEmptyStateWidget)XtParent(child);
    if (request->request_mode & (CWX | CWY | CWBorderWidth | CWSibling | CWStackMode)) return XtGeometryNo;
    if (!state->empty_state.laying_out && !(request->request_mode & XtCWQueryOnly)) negotiate(state);
    reply->request_mode = CWWidth | CWHeight; reply->width = child->core.width; reply->height = child->core.height;
    if (((request->request_mode & CWWidth) && request->width != reply->width) || ((request->request_mode & CWHeight) && request->height != reply->height)) return XtGeometryAlmost;
    return (request->request_mode & XtCWQueryOnly) ? XtGeometryYes : XtGeometryDone;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *intended, XtWidgetGeometry *reply) {
    XmEmptyStateWidget state = (XmEmptyStateWidget)widget; natural(state, &reply->width, &reply->height); reply->request_mode = CWWidth | CWHeight;
    if (intended && (intended->request_mode & (CWWidth | CWHeight)) == (CWWidth | CWHeight) && intended->width == reply->width && intended->height == reply->height) return XtGeometryYes;
    if (state->core.width == reply->width && state->core.height == reply->height) return XtGeometryNo;
    return XtGeometryAlmost;
}
static void activate(Widget widget, XtPointer client, XtPointer call) {
    XmEmptyStateWidget state = (XmEmptyStateWidget)client; XmEmptyStateCallbackStruct callback;
    (void)widget; callback.reason = XmCR_ACTIVATE; callback.event = call ? ((XmPushButtonCallbackStruct *)call)->event : NULL;
    XtCallCallbacks((Widget)state, XmNemptyStateActionCallback, &callback);
}
Widget XmCreateEmptyState(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmEmptyStateWidgetClass, parent, args, count); }
Widget XmVaCreateEmptyState(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmEmptyStateWidgetClass,parent,False,v,n); va_end(v); return w; }
Widget XmVaCreateManagedEmptyState(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmEmptyStateWidgetClass,parent,True,v,n); va_end(v); return w; }
Widget XmEmptyStateGetMessage(Widget widget) { return widget && XmIsEmptyState(widget) ? ((XmEmptyStateWidget)widget)->empty_state.message_widget : NULL; }
Widget XmEmptyStateGetAction(Widget widget) { return widget && XmIsEmptyState(widget) ? ((XmEmptyStateWidget)widget)->empty_state.action_widget : NULL; }
void XmEmptyStateInvokeAction(Widget widget, XEvent *event) { XmEmptyStateCallbackStruct callback; if (!widget || !XmIsEmptyState(widget) || !has_action((XmEmptyStateWidget)widget)) return; callback.reason = XmCR_ACTIVATE; callback.event = event; XtCallCallbacks(widget, XmNemptyStateActionCallback, &callback); }
