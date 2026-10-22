/* Native collapsible sections. LGPL-2.0-or-later. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <Xm/ArrowB.h>
#include <Xm/DisclosureP.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/XmP.h>
#include <limits.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Resize(Widget);
static void ChangeManaged(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *,
                                        XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
#define OFFSET(field) XtOffsetOf(XmDisclosureRec, disclosure.field)
static XtResource resources[] = {
    {XmNexpanded, XmCExpanded, XmRBoolean, sizeof(Boolean), OFFSET(expanded),
     XmRImmediate, (XtPointer)True},
    {XmNlabelString, XmCXmString, XmRXmString, sizeof(XmString), OFFSET(label),
     XmRImmediate, NULL},
    {XmNexpandedCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
     OFFSET(callbacks), XmRImmediate, NULL},
    {XmNdisclosureStateCallback, XmCCallback, XmRCallback,
     sizeof(XtCallbackList), OFFSET(state_callbacks), XmRImmediate, NULL},
    {XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension, sizeof(Dimension),
     OFFSET(margin_width), XmRImmediate, (XtPointer)4},
    {XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension, sizeof(Dimension),
     OFFSET(margin_height), XmRImmediate, (XtPointer)4},
    {XmNspacing, XmCSpacing, XmRVerticalDimension, sizeof(Dimension),
     OFFSET(spacing), XmRImmediate, (XtPointer)4}};
static XmSyntheticResource synthetic_resources[] = {
    {XmNmarginWidth, sizeof(Dimension), OFFSET(margin_width),
     XmeFromHorizontalPixels, XmeToHorizontalPixels},
    {XmNmarginHeight, sizeof(Dimension), OFFSET(margin_height),
     XmeFromVerticalPixels, XmeToVerticalPixels},
    {XmNspacing, sizeof(Dimension), OFFSET(spacing), XmeFromVerticalPixels,
     XmeToVerticalPixels}};
XmDisclosureClassRec xmDisclosureClassRec = {
    .core_class = {.superclass = (WidgetClass)&xmManagerClassRec,
                   .class_name = "XmDisclosure",
                   .widget_size = sizeof(XmDisclosureRec),
                   .initialize = Initialize,
                   .realize = XtInheritRealize,
                   .resources = resources,
                   .num_resources = XtNumber(resources),
                   .compress_motion = True,
                   .compress_exposure = XtExposeCompressMultiple,
                   .compress_enterleave = True,
                   .destroy = Destroy,
                   .resize = Resize,
                   .set_values = SetValues,
                   .set_values_almost = XtInheritSetValuesAlmost,
                   .version = XtVersion,
                   .tm_table = XtInheritTranslations,
                   .query_geometry = QueryGeometry},
    .composite_class = {.geometry_manager = GeometryManager,
                        .change_managed = ChangeManaged,
                        .insert_child = XtInheritInsertChild,
                        .delete_child = XtInheritDeleteChild},
    .constraint_class = {.constraint_size = sizeof(XmManagerConstraintRec)},
    .manager_class = {.translations = XtInheritTranslations,
                      .syn_resources = synthetic_resources,
                      .num_syn_resources = XtNumber(synthetic_resources),
                      .parent_process = XmInheritParentProcess}};
WidgetClass xmDisclosureWidgetClass = (WidgetClass)&xmDisclosureClassRec;

static Dimension dimension(unsigned long value) {
  return (Dimension)(value > USHRT_MAX ? USHRT_MAX : value ? value : 1);
}
static void preferred(Widget child, Dimension *width, Dimension *height) {
  XtWidgetGeometry wanted;
  wanted.request_mode = 0;
  XtQueryGeometry(child, NULL, &wanted);
  *width = wanted.request_mode & CWWidth ? wanted.width : child->core.width;
  *height = wanted.request_mode & CWHeight ? wanted.height : child->core.height;
}
static Boolean right_to_left(XmDisclosureWidget w) {
  return XmDirectionMatchPartial(w->manager.string_direction, XmRIGHT_TO_LEFT,
                                 XmHORIZONTAL_MASK);
}
static unsigned char arrow_direction(XmDisclosureWidget w) {
  return w->disclosure.expanded ? XmARROW_DOWN
         : right_to_left(w)     ? XmARROW_LEFT
                                : XmARROW_RIGHT;
}
static void natural(XmDisclosureWidget w, Dimension *width, Dimension *height) {
  Dimension aw, ah, hw, hh, cw = 0, ch = 0;
  preferred(w->disclosure.arrow, &aw, &ah);
  preferred(w->disclosure.header, &hw, &hh);
  if (w->disclosure.expanded)
    preferred(w->disclosure.content, &cw, &ch);
  *width = dimension(2UL * w->disclosure.margin_width +
                     (aw + hw > cw ? aw + hw : cw));
  *height =
      dimension(2UL * w->disclosure.margin_height + (ah > hh ? ah : hh) +
                (w->disclosure.expanded ? w->disclosure.spacing + ch : 0));
}
static void layout(XmDisclosureWidget w) {
  Dimension aw, ah, hw, hh, width, height, head;
  Position x, y;
  if (w->disclosure.initializing || w->disclosure.laying_out)
    return;
  w->disclosure.laying_out = True;
  preferred(w->disclosure.arrow, &aw, &ah);
  preferred(w->disclosure.header, &hw, &hh);
  head = ah > hh ? ah : hh;
  x = (Position)w->disclosure.margin_width;
  y = (Position)w->disclosure.margin_height;
  width = dimension(w->core.width > 2UL * w->disclosure.margin_width
                        ? w->core.width - 2UL * w->disclosure.margin_width
                        : 1);
  if (aw >= width)
    aw = width > 1 ? width - 1 : 1;
  XmeConfigureObject(w->disclosure.arrow,
                     right_to_left(w) ? (Position)(x + width - aw) : x, y, aw,
                     head, 0);
  XmeConfigureObject(w->disclosure.header,
                     right_to_left(w) ? x : (Position)(x + aw), y,
                     dimension(width > aw ? width - aw : 1), head, 0);
  y = (Position)(y + head + w->disclosure.spacing);
  height =
      dimension(w->core.height > (unsigned long)y + w->disclosure.margin_height
                    ? w->core.height - y - w->disclosure.margin_height
                    : 1);
  if (w->disclosure.expanded)
    XmeConfigureObject(w->disclosure.content, x, y, width, height, 0);
  w->disclosure.laying_out = False;
}
static void negotiate(XmDisclosureWidget w) {
  Dimension width, height, reply_width, reply_height;
  XtGeometryResult result;
  if (w->disclosure.initializing || w->disclosure.laying_out)
    return;
  natural(w, &width, &height);
  result = XtMakeResizeRequest((Widget)w, width, height, &reply_width,
                               &reply_height);
  if (result == XtGeometryAlmost)
    XtMakeResizeRequest((Widget)w, reply_width, reply_height, NULL, NULL);
  layout(w);
}
static void transition(XmDisclosureWidget w, XEvent *event, Boolean notify) {
  XmDisclosureCallbackStruct cb;
  Widget shell = (Widget)w, focus;
  while (shell && !XtIsShell(shell))
    shell = XtParent(shell);
  focus = shell ? XmGetFocusWidget(shell) : NULL;
  if (!w->disclosure.expanded) {
    Widget ancestor = focus;
    while (ancestor && ancestor != w->disclosure.content)
      ancestor = XtParent(ancestor);
    if (ancestor &&
        !XmProcessTraversal(w->disclosure.header, XmTRAVERSE_CURRENT))
      XmProcessTraversal((Widget)w, XmTRAVERSE_NEXT_TAB_GROUP);
  }
  XtVaSetValues(w->disclosure.arrow, XmNarrowDirection, arrow_direction(w),
                NULL);
  XtSetMappedWhenManaged(w->disclosure.content, w->disclosure.expanded);
  negotiate(w);
  cb.reason = XmCR_VALUE_CHANGED;
  cb.event = event;
  cb.expanded = w->disclosure.expanded;
  XtCallCallbackList((Widget)w, w->disclosure.state_callbacks, &cb);
  if (notify) {
    XtCallCallbackList((Widget)w, w->disclosure.callbacks, &cb);
  }
}
static void activate(Widget child, XtPointer data, XtPointer call) {
  XmDisclosureWidget w = (XmDisclosureWidget)data;
  XmAnyCallbackStruct *cb = (XmAnyCallbackStruct *)call;
  (void)child;
  w->disclosure.expanded = !w->disclosure.expanded;
  transition(w, cb ? cb->event : NULL, True);
}
static void key(Widget child, XtPointer data, XEvent *event,
                Boolean *dispatch) {
  XmDisclosureWidget w = (XmDisclosureWidget)data;
  KeySym symbol;
  Boolean expanded;
  (void)child;
  if (event->type != KeyPress)
    return;
  symbol = XLookupKeysym(&event->xkey, 0);
  if (symbol == XK_space || symbol == XK_Return || symbol == XK_KP_Enter) {
    *dispatch = False;
    w->disclosure.expanded = !w->disclosure.expanded;
    transition(w, event, True);
    return;
  }
  if (symbol != XK_Left && symbol != XK_Right)
    return;
  expanded = right_to_left(w) ? symbol == XK_Left : symbol == XK_Right;
  *dispatch = False;
  if (expanded != w->disclosure.expanded) {
    w->disclosure.expanded = expanded;
    transition(w, event, True);
  }
}
static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
  XmDisclosureWidget w = (XmDisclosureWidget)widget;
  Dimension width, height;
  (void)request;
  (void)args;
  (void)count;
  w->disclosure.initializing = True;
  w->disclosure.laying_out = False;
  w->disclosure.label = w->disclosure.label
                            ? XmStringCopy(w->disclosure.label)
                            : XmStringCreateLocalized(XtName(widget));
  w->disclosure.arrow = XtVaCreateManagedWidget(
      "indicator", xmArrowButtonWidgetClass, widget, XmNarrowDirection,
      arrow_direction(w), XmNtraversalOn, False, XmNwidth, 24, XmNheight, 24,
      XmNborderWidth, 0, NULL);
  w->disclosure.header =
      XtVaCreateManagedWidget("header", xmPushButtonWidgetClass, widget,
                              XmNlabelString, w->disclosure.label, XmNalignment,
                              XmALIGNMENT_BEGINNING, XmNborderWidth, 0, NULL);
  w->disclosure.content = XtVaCreateManagedWidget(
      "content", xmRowColumnWidgetClass, widget, XmNorientation, XmVERTICAL,
      XmNpacking, XmPACK_TIGHT, XmNborderWidth, 0, XmNmappedWhenManaged,
      w->disclosure.expanded, NULL);
  XtAddCallback(w->disclosure.header, XmNactivateCallback, activate, w);
  XtAddCallback(w->disclosure.arrow, XmNactivateCallback, activate, w);
  XtAddEventHandler(w->disclosure.header, KeyPressMask, False, key, w);
  w->disclosure.initializing = False;
  natural(w, &width, &height);
  if (!w->core.width)
    w->core.width = width;
  if (!w->core.height)
    w->core.height = height;
  layout(w);
}
static void Destroy(Widget widget) {
  XmStringFree(((XmDisclosureWidget)widget)->disclosure.label);
}
static void Resize(Widget widget) { layout((XmDisclosureWidget)widget); }
static void ChangeManaged(Widget widget) {
  negotiate((XmDisclosureWidget)widget);
}
static Boolean SetValues(Widget old, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
  XmDisclosureWidget before = (XmDisclosureWidget)old,
                     w = (XmDisclosureWidget)widget;
  Boolean changed = before->disclosure.expanded != w->disclosure.expanded;
  (void)request;
  (void)args;
  (void)count;
  if (before->disclosure.label != w->disclosure.label) {
    w->disclosure.label = w->disclosure.label
                              ? XmStringCopy(w->disclosure.label)
                              : XmStringCreateLocalized(XtName(widget));
    XmStringFree(before->disclosure.label);
    XtVaSetValues(w->disclosure.header, XmNlabelString, w->disclosure.label,
                  NULL);
  }
  XtVaSetValues(w->disclosure.arrow, XmNarrowDirection, arrow_direction(w),
                NULL);
  if (changed)
    transition(w, NULL, True);
  else
    negotiate(w);
  return False;
}
static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request,
                                        XtWidgetGeometry *reply) {
  XmDisclosureWidget w = (XmDisclosureWidget)XtParent(child);
  if (request->request_mode &
      (CWX | CWY | CWBorderWidth | CWSibling | CWStackMode))
    return XtGeometryNo;
  if (w->disclosure.laying_out)
    return XtGeometryNo;
  if (!(request->request_mode & XtCWQueryOnly))
    negotiate(w);
  reply->request_mode = CWWidth | CWHeight;
  reply->width = child->core.width;
  reply->height = child->core.height;
  if (((request->request_mode & CWWidth) && request->width != reply->width) ||
      ((request->request_mode & CWHeight) && request->height != reply->height))
    return XtGeometryAlmost;
  return (request->request_mode & XtCWQueryOnly) ? XtGeometryYes
                                                 : XtGeometryDone;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *intended,
                                      XtWidgetGeometry *preferred_geometry) {
  XmDisclosureWidget w = (XmDisclosureWidget)widget;
  natural(w, &preferred_geometry->width, &preferred_geometry->height);
  preferred_geometry->request_mode = CWWidth | CWHeight;
  if ((intended->request_mode & (CWWidth | CWHeight)) == (CWWidth | CWHeight) &&
      intended->width == preferred_geometry->width &&
      intended->height == preferred_geometry->height)
    return XtGeometryYes;
  if (w->core.width == preferred_geometry->width &&
      w->core.height == preferred_geometry->height)
    return XtGeometryNo;
  return XtGeometryAlmost;
}
Widget XmCreateDisclosure(Widget parent, char *name, ArgList args,
                          Cardinal count) {
  return XtCreateWidget(name, xmDisclosureWidgetClass, parent, args, count);
}
Widget XmDisclosureGetContent(Widget w) {
  return w && XmIsDisclosure(w) ? ((XmDisclosureWidget)w)->disclosure.content
                                : NULL;
}
Widget XmDisclosureGetHeader(Widget w) {
  return w && XmIsDisclosure(w) ? ((XmDisclosureWidget)w)->disclosure.header
                                : NULL;
}
Boolean XmDisclosureGetExpanded(Widget w) {
  return w && XmIsDisclosure(w) ? ((XmDisclosureWidget)w)->disclosure.expanded
                                : False;
}
void XmDisclosureSetExpanded(Widget widget, Boolean expanded, Boolean notify) {
  XmDisclosureWidget w;
  if (!widget || !XmIsDisclosure(widget))
    return;
  w = (XmDisclosureWidget)widget;
  expanded = expanded ? True : False;
  if (expanded != w->disclosure.expanded) {
    w->disclosure.expanded = expanded;
    transition(w, NULL, notify);
  }
}
