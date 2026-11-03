#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Xm/ArrowB.h>
#include <Xm/Label.h>
#include <Xm/Appearance.h>
#include "XmI.h"
#include <Xm/RowColumn.h>
#include <Xm/SidebarP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <X11/keysym.h>
#include <X11/StringDefs.h>
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
static void child_created(Widget, XtPointer, XtPointer);
static void set_expanded(Widget, Boolean, XEvent *, Boolean);
static void saved_focus_destroyed(Widget, XtPointer, XtPointer);

#define Offset(field) XtOffsetOf(XmSidebarRec, sidebar.field)
static XtResource resources[] = {
    {XmNlabelString, XmCLabelString, XmRXmString, sizeof(XmString),
     Offset(title), XmRString, (XtPointer)"Sidebar"},
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
    if (width == NULL || height == NULL)
        return;
    if (child == NULL) {
        *width = 0;
        *height = 0;
        return;
    }
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

static void header_metrics(XmSidebarWidget sidebar, Dimension *button, Dimension *gap)
{
    Dimension title_width, title_height;
    XmAppearanceDensity density;
    preferred(sidebar->sidebar.title_label, &title_width, &title_height);
    XmGetAppearanceSpacing(XtDisplay((Widget)sidebar), XmAPPEARANCE_SPACING_SMALL, gap);
    XmGetAppearanceDensity(XtDisplay((Widget)sidebar), &density);
    *button = (Dimension)MIN(USHRT_MAX,MAX(_XmDpiDefaultDimension((Widget)sidebar,
        density == XmAPPEARANCE_DENSITY_COMPACT ? 28 : 34), (unsigned long)title_height + 2 * *gap));
}

static void natural(XmSidebarWidget sidebar, Dimension *width, Dimension *height)
{
    Dimension button, gap, content_width, content_height;
    unsigned long extent;
    header_metrics(sidebar, &button, &gap);
    preferred(sidebar->sidebar.content, &content_width, &content_height);
    *width = (Dimension)MIN(USHRT_MAX, MAX(
        sidebar->sidebar.expanded ? sidebar->sidebar.expanded_width : sidebar->sidebar.collapsed_width,
        (unsigned long)button + 2 * gap));
    extent = (unsigned long)button + 2 * gap +
        (sidebar->sidebar.expanded ? content_height : 0);
    *height = (Dimension)MIN(MAX(extent,1),USHRT_MAX);
    if (!*width) *width = 1;
}

static void layout(XmSidebarWidget sidebar)
{
    Dimension button, gap, available;
    int width, height, x, y, remaining, label_width;
    Boolean rtl;
    if (sidebar->sidebar.initializing || sidebar->sidebar.laying_out)
        return;
    if (sidebar->sidebar.toggle_button == NULL ||
        sidebar->sidebar.title_label == NULL || sidebar->sidebar.content == NULL)
        return;
    sidebar->sidebar.laying_out = True;
    header_metrics(sidebar, &button, &gap);
    width = MIN(sidebar->core.width, SHRT_MAX);
    height = MIN(sidebar->core.height, SHRT_MAX);
    rtl = XmDirectionMatch(_XmGetLayoutDirection((Widget)sidebar), XmRIGHT_TO_LEFT);
    available = (Dimension)MIN((unsigned long)width, (unsigned long)height);
    button = MIN(button, available);
    if (!button) {
        XtSetMappedWhenManaged(sidebar->sidebar.toggle_button,False);
        XtUnmanageChild(sidebar->sidebar.title_label);
        XtUnmanageChild(sidebar->sidebar.content);
        sidebar->sidebar.laying_out = False;
        return;
    }
    y = MIN((int)gap, height-(int)button);
    x = sidebar->sidebar.expanded ?
        (rtl ? MIN((int)gap,width-(int)button) : MAX(0,width-(int)gap-(int)button)) :
        (width-(int)button)/2;
    XtSetMappedWhenManaged(sidebar->sidebar.toggle_button,True);
    XtVaSetValues(sidebar->sidebar.toggle_button,XmNarrowDirection,
        sidebar->sidebar.expanded != rtl ? XmARROW_LEFT : XmARROW_RIGHT,NULL);
    XmeConfigureObject(sidebar->sidebar.toggle_button,x,y,button,button,0);
    label_width = width - button - 3 * (int)gap;
    if (sidebar->sidebar.expanded && label_width > 0) {
        XtManageChild(sidebar->sidebar.title_label);
        XmeConfigureObject(sidebar->sidebar.title_label,
            rtl ? button + 2*gap : gap,y,label_width,button,0);
    } else XtUnmanageChild(sidebar->sidebar.title_label);
    remaining = height - y - button - gap;
    XtSetMappedWhenManaged(sidebar->sidebar.content,sidebar->sidebar.expanded && remaining>0);
    if (sidebar->sidebar.expanded && remaining>0) {
        XtManageChild(sidebar->sidebar.content);
        XmeConfigureObject(sidebar->sidebar.content,0,y+button+gap,width,remaining,0);
    } else XtUnmanageChild(sidebar->sidebar.content);
    sidebar->sidebar.laying_out = False;
}

static void negotiate(XmSidebarWidget sidebar)
{
    Dimension width, height, reply_width, reply_height;
    XtGeometryResult result;
    if (sidebar->sidebar.initializing || sidebar->sidebar.laying_out)
        return;
    natural(sidebar, &width, &height);
    /* A sidebar collapses along its horizontal axis. The parent owns the
     * vertical allocation; using the hidden content's preferred height here
     * shrinks the whole window and clips content when it reopens. */
    height = sidebar->core.height;
    result = XtMakeResizeRequest((Widget)sidebar, width, height, &reply_width,
                                 &reply_height);
    if (result == XtGeometryAlmost)
        XtMakeResizeRequest((Widget)sidebar, reply_width, reply_height, NULL, NULL);
    layout(sidebar);
}

static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count)
{
    XmSidebarWidget sidebar = (XmSidebarWidget)widget;
    sidebar->sidebar.title = sidebar->sidebar.title ?
        XmStringCopy(sidebar->sidebar.title) : XmStringCreateLocalized("Sidebar");
    (void)request;
    (void)args;
    (void)count;
    sidebar->sidebar.initializing = True;
    sidebar->sidebar.laying_out = False;
    sidebar->sidebar.saved_focus = NULL;
    /* Xt dispatches key events directly to the focused descendant, without
     * bubbling through managers. Register each descendant when it is created,
     * including application children added after this composite is realized. */
    XtAddCallback(XtHooksOfDisplay(XtDisplay(widget)), XtNcreateHook,
                  child_created, widget);
    sidebar->sidebar.content = XmCreateRowColumn(widget, (char *)"content",
                                                 NULL, 0);
    if (sidebar->sidebar.content != NULL)
        XtManageChild(sidebar->sidebar.content);
    sidebar->sidebar.title_label = XtVaCreateManagedWidget(
        "title",xmLabelWidgetClass,widget,XmNlabelString,sidebar->sidebar.title,
        XmNalignment,XmALIGNMENT_BEGINNING,NULL);
    sidebar->sidebar.toggle_button = XtVaCreateManagedWidget(
        "toggle", xmArrowButtonWidgetClass, widget, NULL);
    if (sidebar->sidebar.toggle_button != NULL)
        XtAddCallback(sidebar->sidebar.toggle_button, XmNactivateCallback, activate,
                      sidebar);
    XtInsertEventHandler(widget, KeyPressMask, False, key_handler, widget, XtListHead);
    sidebar->sidebar.initializing = False;
    {
        Dimension width, height;
        natural(sidebar, &width, &height);
        if (!request->core.width) sidebar->core.width = width;
        if (!request->core.height) sidebar->core.height = height;
    }
}

static void Destroy(Widget widget)
{
    XmSidebarWidget sidebar=(XmSidebarWidget)widget;
    XmStringFree(sidebar->sidebar.title);
    if (sidebar->sidebar.saved_focus)
        XtRemoveCallback(sidebar->sidebar.saved_focus,XmNdestroyCallback,saved_focus_destroyed,sidebar);
    XtRemoveCallback(XtHooksOfDisplay(XtDisplay(widget)), XtNcreateHook,
                     child_created, widget);
}

static void Resize(Widget widget) { layout((XmSidebarWidget)widget); }
static void ChangeManaged(Widget widget) { negotiate((XmSidebarWidget)widget); }

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count)
{
    XmSidebarWidget old = (XmSidebarWidget)old_widget;
    XmSidebarWidget sidebar = (XmSidebarWidget)widget;
    Boolean title_changed = False;
    (void)request;
    (void)args;
    (void)count;
    if (old->sidebar.title != sidebar->sidebar.title) {
        XmString replacement = sidebar->sidebar.title ?
            XmStringCopy(sidebar->sidebar.title) : XmStringCreateLocalized("Sidebar");
        if (replacement != NULL) {
            XmStringFree(old->sidebar.title);
            sidebar->sidebar.title = replacement;
            if (sidebar->sidebar.title_label != NULL)
                XtVaSetValues(sidebar->sidebar.title_label,
                              XmNlabelString, sidebar->sidebar.title, NULL);
            layout(sidebar);
            title_changed = True;
        } else {
            /* Keep the valid, owned title when a replacement cannot be made. */
            sidebar->sidebar.title = old->sidebar.title;
        }
    }
    if (old->sidebar.expanded != sidebar->sidebar.expanded) {
        Boolean expanded=sidebar->sidebar.expanded;
        sidebar->sidebar.expanded=old->sidebar.expanded;
        set_expanded(widget,expanded,NULL,False);
        return True;
    }
    if (old->sidebar.expanded_width != sidebar->sidebar.expanded_width ||
        old->sidebar.collapsed_width != sidebar->sidebar.collapsed_width) {
        layout(sidebar);
        return True;
    }
    return title_changed;
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
    if (sidebar == NULL || !XmIsSidebar((Widget)sidebar))
        return;
    XmSidebarToggle((Widget)sidebar,
                    call ? ((XmAnyCallbackStruct *)call)->event : NULL);
    (void)widget;
}

static void child_created(Widget hooks, XtPointer client, XtPointer call)
{
    XtCreateHookData created = (XtCreateHookData)call;
    Widget owner;
    (void)hooks;
    if (!created || !XtIsWidget(created->widget)) return;
    owner = created->widget;
    while (owner && !XmIsSidebar(owner) && !XtIsShell(owner)) owner = XtParent(owner);
    if (owner == (Widget)client)
        XtInsertEventHandler(created->widget, KeyPressMask, False,
                             key_handler, client, XtListHead);
}

static void key_handler(Widget widget, XtPointer client, XEvent *event,
                        Boolean *dispatch)
{
    KeySym key;
    char input[2];
    if (widget == NULL || client == NULL || event == NULL ||
        !XmIsSidebar((Widget)client) ||
        event->type != KeyPress || !XtIsSensitive(widget) ||
        !XtIsSensitive((Widget)client))
        return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if ((key == XK_m || key == XK_M) && (event->xkey.state & ControlMask)) {
        XmSidebarToggle((Widget)client, event);
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
    set_expanded(widget, expanded, NULL, True);
}

static void saved_focus_destroyed(Widget widget, XtPointer client, XtPointer call)
{
    XmSidebarWidget sidebar=(XmSidebarWidget)client;
    (void)widget; (void)call;
    sidebar->sidebar.saved_focus=NULL;
}

static void set_expanded(Widget widget, Boolean expanded, XEvent *event, Boolean notify)
{
    XmSidebarWidget sidebar;
    Widget focus;
    Boolean focus_inside;
    XmSidebarCallbackStruct callback;
    if (!widget || !XmIsSidebar(widget) || widget->core.being_destroyed)
        return;
    sidebar = (XmSidebarWidget)widget;
    if (sidebar->sidebar.expanded == expanded)
        return;
    focus = XmGetFocusWidget(widget);
    focus_inside = focus && descendant(focus, widget);
    if (!expanded) {
        if (sidebar->sidebar.saved_focus)
            XtRemoveCallback(sidebar->sidebar.saved_focus,XmNdestroyCallback,saved_focus_destroyed,sidebar);
        sidebar->sidebar.saved_focus=NULL;
        if (focus && descendant(focus, sidebar->sidebar.content)) {
            sidebar->sidebar.saved_focus = focus;
            XtAddCallback(focus,XmNdestroyCallback,saved_focus_destroyed,sidebar);
        }
    }
    sidebar->sidebar.expanded = expanded;
    negotiate(sidebar);
    /* Resizing can temporarily put the old toggle position outside the new
     * allocation, invalidating traversal. Focus its final geometry instead. */
    if (!expanded && focus_inside && sidebar->sidebar.toggle_button != NULL)
        XmProcessTraversal(sidebar->sidebar.toggle_button, XmTRAVERSE_CURRENT);
    if (expanded && sidebar->sidebar.saved_focus) {
        Widget restore=sidebar->sidebar.saved_focus;
        XtRemoveCallback(restore,XmNdestroyCallback,saved_focus_destroyed,sidebar);
        sidebar->sidebar.saved_focus = NULL;
        if (focus_inside) XmProcessTraversal(restore, XmTRAVERSE_CURRENT);
    }
    if (sidebar->core.being_destroyed)
        return;
    callback.reason = XmSIDEBAR_REASON_TOGGLED;
    callback.event = event;
    callback.expanded = expanded;
    if (notify) XtCallCallbacks(widget, XmNsidebarToggleCallback, &callback);
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
    set_expanded(widget, !sidebar->sidebar.expanded, event, True);
}
