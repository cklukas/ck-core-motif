#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/SidebarP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <X11/keysym.h>
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
static void activate(Widget, XtPointer, XtPointer);
static void key_handler(Widget, XtPointer, XEvent *, Boolean *);
static void set_expanded(Widget, Boolean, XEvent *);

#define Offset(field) XtOffsetOf(XmSidebarRec, sidebar.field)
static XtResource resources[] = {
    {XmNsidebarExpandedWidth, XmCSidebarExpandedWidth, XmRHorizontalDimension,
     sizeof(Dimension), Offset(expanded_width), XmRImmediate, (XtPointer)240},
    {XmNsidebarCollapsedWidth, XmCSidebarCollapsedWidth,
     XmRHorizontalDimension, sizeof(Dimension), Offset(collapsed_width),
     XmRImmediate, (XtPointer)40},
    {XmNsidebarExpanded, XmCSidebarExpanded, XmRBoolean, sizeof(Boolean),
     Offset(expanded), XmRImmediate, (XtPointer)True},
    {XmNsidebarToggleCallback, XmCSidebarToggleCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(toggle_callback), XmRPointer, NULL}
};

externaldef(xmsidebarclassrec) XmSidebarClassRec xmSidebarClassRec = {
    {.superclass = (WidgetClass)&xmManagerClassRec,
     .class_name = "XmSidebar", .widget_size = sizeof(XmSidebarRec),
     .initialize = Initialize, .realize = XtInheritRealize,
     .resources = resources, .num_resources = XtNumber(resources),
     .compress_motion = True, .compress_exposure = XtExposeCompressMultiple,
     .compress_enterleave = True, .destroy = Destroy, .resize = Resize,
     .set_values = SetValues, .set_values_almost = XtInheritSetValuesAlmost,
     .version = XtVersion, .tm_table = XtInheritTranslations,
     .query_geometry = QueryGeometry},
    {.geometry_manager = GeometryManager, .change_managed = ChangeManaged,
     .insert_child = XtInheritInsertChild, .delete_child = XtInheritDeleteChild},
    {.constraint_size = sizeof(XmManagerConstraintRec)},
    {.translations = XtInheritTranslations,
     .parent_process = XmInheritParentProcess},
    {NULL}};

externaldef(xmsidebarwidgetclass) WidgetClass xmSidebarWidgetClass =
    (WidgetClass)&xmSidebarClassRec;

static void preferred(Widget child, Dimension *width, Dimension *height)
{
    XtWidgetGeometry wanted;
    wanted.request_mode = 0;
    XtQueryGeometry(child, NULL, &wanted);
    *width = wanted.request_mode & CWWidth ? wanted.width : child->core.width;
    *height = wanted.request_mode & CWHeight ? wanted.height : child->core.height;
}

static Boolean descendant(Widget child, Widget ancestor)
{
    while (child) {
        if (child == ancestor)
            return True;
        child = XtParent(child);
    }
    return False;
}

static void natural(XmSidebarWidget sidebar, Dimension *width, Dimension *height)
{
    Dimension toggle_width, toggle_height, content_width, content_height;
    preferred(sidebar->sidebar.toggle_button, &toggle_width, &toggle_height);
    preferred(sidebar->sidebar.content, &content_width, &content_height);
    (void)toggle_width;
    *width = sidebar->sidebar.expanded ? sidebar->sidebar.expanded_width
                                       : sidebar->sidebar.collapsed_width;
    *height = toggle_height + (sidebar->sidebar.expanded ? content_height : 0);
    if (*height == 0)
        *height = 1;
}

static void layout(XmSidebarWidget sidebar)
{
    Dimension toggle_width, toggle_height;
    if (sidebar->sidebar.initializing || sidebar->sidebar.laying_out)
        return;
    sidebar->sidebar.laying_out = True;
    preferred(sidebar->sidebar.toggle_button, &toggle_width, &toggle_height);
    XtSetMappedWhenManaged(sidebar->sidebar.content, sidebar->sidebar.expanded);
    XmeConfigureObject(sidebar->sidebar.toggle_button, 0, 0,
                       sidebar->core.width, toggle_height, 0);
    if (sidebar->sidebar.expanded) {
        XtManageChild(sidebar->sidebar.content);
        XmeConfigureObject(sidebar->sidebar.content, 0, toggle_height,
                           sidebar->core.width,
                           sidebar->core.height > toggle_height
                               ? sidebar->core.height - toggle_height : 1,
                           0);
    } else {
        XtUnmanageChild(sidebar->sidebar.content);
    }
    sidebar->sidebar.laying_out = False;
}

static void negotiate(XmSidebarWidget sidebar)
{
    Dimension width, height, reply_width, reply_height;
    XtGeometryResult result;
    if (sidebar->sidebar.initializing || sidebar->sidebar.laying_out)
        return;
    natural(sidebar, &width, &height);
    result = XtMakeResizeRequest((Widget)sidebar, width, height, &reply_width,
                                 &reply_height);
    if (result == XtGeometryAlmost)
        XtMakeResizeRequest((Widget)sidebar, reply_width, reply_height, NULL, NULL);
    layout(sidebar);
}

static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count)
{
    XmSidebarWidget sidebar = (XmSidebarWidget)widget;
    XmString label = XmStringCreateLocalized((char *)(sidebar->sidebar.expanded
                                                           ? "Collapse"
                                                           : "Expand"));
    (void)request;
    (void)args;
    (void)count;
    sidebar->sidebar.initializing = True;
    sidebar->sidebar.laying_out = False;
    sidebar->sidebar.saved_focus = NULL;
    sidebar->sidebar.content = XmCreateRowColumn(widget, (char *)"content",
                                                 NULL, 0);
    XtManageChild(sidebar->sidebar.content);
    sidebar->sidebar.toggle_button = XtVaCreateManagedWidget(
        "toggle", xmPushButtonWidgetClass, widget, XmNlabelString, label,
        XmNborderWidth, 0, NULL);
    XtAddCallback(sidebar->sidebar.toggle_button, XmNactivateCallback, activate,
                  sidebar);
    XtAddEventHandler(widget, KeyPressMask, True, key_handler, NULL);
    XmStringFree(label);
    sidebar->sidebar.initializing = False;
}

static void Destroy(Widget widget)
{
    XtRemoveEventHandler(widget, KeyPressMask, True, key_handler, NULL);
}

static void Resize(Widget widget) { layout((XmSidebarWidget)widget); }
static void ChangeManaged(Widget widget) { negotiate((XmSidebarWidget)widget); }

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count)
{
    XmSidebarWidget old = (XmSidebarWidget)old_widget;
    XmSidebarWidget sidebar = (XmSidebarWidget)widget;
    (void)request;
    (void)args;
    (void)count;
    if (old->sidebar.expanded != sidebar->sidebar.expanded ||
        old->sidebar.expanded_width != sidebar->sidebar.expanded_width ||
        old->sidebar.collapsed_width != sidebar->sidebar.collapsed_width) {
        layout(sidebar);
        return True;
    }
    return False;
}

static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request,
                                        XtWidgetGeometry *reply)
{
    XmSidebarWidget sidebar = (XmSidebarWidget)XtParent(child);
    if (request->request_mode & (CWX | CWY | CWBorderWidth | CWSibling | CWStackMode))
        return XtGeometryNo;
    if (!sidebar->sidebar.laying_out && !(request->request_mode & XtCWQueryOnly))
        negotiate(sidebar);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = child->core.width;
    reply->height = child->core.height;
    return XtGeometryAlmost;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply)
{
    XmSidebarWidget sidebar = (XmSidebarWidget)widget;
    natural(sidebar, &reply->width, &reply->height);
    reply->request_mode = CWWidth | CWHeight;
    if (request && request->width == reply->width && request->height == reply->height)
        return XtGeometryYes;
    return XtGeometryAlmost;
}

static void activate(Widget widget, XtPointer client, XtPointer call)
{
    XmSidebarWidget sidebar = (XmSidebarWidget)client;
    XmSidebarToggle((Widget)sidebar,
                    call ? ((XmPushButtonCallbackStruct *)call)->event : NULL);
    (void)widget;
}

static void key_handler(Widget widget, XtPointer client, XEvent *event,
                        Boolean *dispatch)
{
    KeySym key;
    char input[2];
    (void)client;
    if (event->type != KeyPress)
        return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_m && (event->xkey.state & ControlMask)) {
        XmSidebarToggle(widget, event);
        if (dispatch)
            *dispatch = False;
    }
}

Widget XmCreateSidebar(Widget parent, char *name, ArgList args, Cardinal count)
{
    return XtCreateWidget(name, xmSidebarWidgetClass, parent, args, count);
}

Widget XmVaCreateSidebar(Widget parent, char *name, ...)
{
    Widget widget;
    va_list values;
    int count;
    Va_start(values, name);
    count = XmeCountVaListSimple(values);
    va_end(values);
    Va_start(values, name);
    widget = XmeVLCreateWidget(name, xmSidebarWidgetClass, parent, False,
                               values, count);
    va_end(values);
    return widget;
}

Widget XmVaCreateManagedSidebar(Widget parent, char *name, ...)
{
    Widget widget;
    va_list values;
    int count;
    Va_start(values, name);
    count = XmeCountVaListSimple(values);
    va_end(values);
    Va_start(values, name);
    widget = XmeVLCreateWidget(name, xmSidebarWidgetClass, parent, True,
                               values, count);
    va_end(values);
    return widget;
}

Widget XmSidebarGetContent(Widget widget)
{
    return widget && XmIsSidebar(widget)
               ? ((XmSidebarWidget)widget)->sidebar.content : NULL;
}

void XmSidebarSetExpanded(Widget widget, Boolean expanded)
{
    set_expanded(widget, expanded, NULL);
}

static void set_expanded(Widget widget, Boolean expanded, XEvent *event)
{
    XmSidebarWidget sidebar;
    Widget focus;
    XmString label;
    XmSidebarCallbackStruct callback;
    if (!widget || !XmIsSidebar(widget))
        return;
    sidebar = (XmSidebarWidget)widget;
    if (sidebar->sidebar.expanded == expanded)
        return;
    if (!expanded) {
        focus = XmGetFocusWidget(widget);
        if (focus && descendant(focus, sidebar->sidebar.content))
            sidebar->sidebar.saved_focus = focus;
        XmProcessTraversal(sidebar->sidebar.toggle_button, XmTRAVERSE_CURRENT);
    }
    sidebar->sidebar.expanded = expanded;
    label = XmStringCreateLocalized((char *)(expanded ? "Collapse" : "Expand"));
    XtVaSetValues(sidebar->sidebar.toggle_button, XmNlabelString, label, NULL);
    XmStringFree(label);
    layout(sidebar);
    if (expanded && sidebar->sidebar.saved_focus) {
        XmProcessTraversal(sidebar->sidebar.saved_focus, XmTRAVERSE_CURRENT);
        sidebar->sidebar.saved_focus = NULL;
    }
    callback.reason = XmSIDEBAR_REASON_TOGGLED;
    callback.event = event;
    callback.expanded = expanded;
    XtCallCallbacks(widget, XmNsidebarToggleCallback, &callback);
}

Boolean XmSidebarGetExpanded(Widget widget)
{
    return widget && XmIsSidebar(widget)
               ? ((XmSidebarWidget)widget)->sidebar.expanded : False;
}

void XmSidebarToggle(Widget widget, XEvent *event)
{
    XmSidebarWidget sidebar;
    if (!widget || !XmIsSidebar(widget))
        return;
    sidebar = (XmSidebarWidget)widget;
    set_expanded(widget, !sidebar->sidebar.expanded, event);
}
