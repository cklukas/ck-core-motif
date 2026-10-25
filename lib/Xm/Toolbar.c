#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToolbarP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

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

#define Offset(field) XtOffsetOf(XmToolbarRec, toolbar.field)
static XtResource resources[] = {
    {XmNtoolbarSpacing, XmCToolbarSpacing, XmRHorizontalDimension,
     sizeof(Dimension), Offset(spacing), XmRImmediate, (XtPointer)4},
    {XmNtoolbarOverflowLabel, XmCToolbarOverflowLabel, XmRString,
     sizeof(String), Offset(overflow_label), XmRImmediate, (XtPointer)"More"},
    {XmNtoolbarActivateCallback, XmCToolbarActivateCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(activate_callback), XmRPointer, NULL}
};

externaldef(xmtoolbarclassrec) XmToolbarClassRec xmToolbarClassRec = {
    {.superclass = (WidgetClass)&xmManagerClassRec,
     .class_name = "XmToolbar", .widget_size = sizeof(XmToolbarRec),
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

externaldef(xmtoolbarwidgetclass) WidgetClass xmToolbarWidgetClass =
    (WidgetClass)&xmToolbarClassRec;

static char *text_copy(XmString value)
{
    char *text = value ? XmStringUnparse(value, NULL, XmCHARSET_TEXT,
                                          XmCHARSET_TEXT, NULL, 0,
                                          XmOUTPUT_ALL) : NULL;
    return text ? text : XtNewString("");
}

static XmString string_copy(XmString value)
{
    char *text = text_copy(value);
    XmString result;
    if (!text)
	return NULL;
    result = XmStringCreateLocalized(text);
    XtFree(text);
    return result;
}

static XmString action_label(const XmToolbarAction *action)
{
    char *label = text_copy(action->label);
    char *shortcut = text_copy(action->shortcut);
    size_t label_length, shortcut_length, size;
    char *text;
    XmString result;

    if (!label || !shortcut) {
	XtFree(label);
	XtFree(shortcut);
	return NULL;
    }
    label_length = strlen(label);
    shortcut_length = strlen(shortcut);
    if (shortcut[0]) {
	if (label_length > SIZE_MAX - 3 ||
	    shortcut_length > SIZE_MAX - 3 - label_length) {
	    XtFree(label);
	    XtFree(shortcut);
	    return NULL;
	}
	size = label_length + shortcut_length + 3;
    } else {
	if (label_length == SIZE_MAX) {
	    XtFree(label);
	    XtFree(shortcut);
	    return NULL;
	}
	size = label_length + 1;
    }
    if (size > UINT_MAX) {
	XtFree(label);
	XtFree(shortcut);
	return NULL;
    }
    text = XtMalloc((Cardinal)size);
    if (!text) {
	XtFree(label);
	XtFree(shortcut);
	return NULL;
    }

    strcpy(text, label);
    if (shortcut[0]) {
        strcat(text, "  ");
        strcat(text, shortcut);
    }
    result = XmStringCreateLocalized(text);
    XtFree(text);
    XtFree(label);
    XtFree(shortcut);
    return result;
}

static void free_actions(XmToolbarWidget toolbar, Boolean destroy_children)
{
    XmToolbarStoredAction *actions = toolbar->toolbar.actions;
    Cardinal count = toolbar->toolbar.action_count;
    Cardinal i;

    toolbar->toolbar.actions = NULL;
    toolbar->toolbar.action_count = 0;
    toolbar->toolbar.visible_count = 0;
    toolbar->toolbar.overflow_count = 0;
    for (i = 0; i < count; ++i) {
        if (destroy_children) {
            if (actions[i].button)
                XtDestroyWidget(actions[i].button);
            if (actions[i].menu_button)
                XtDestroyWidget(actions[i].menu_button);
        }
        XtFree(actions[i].id);
        XmStringFree(actions[i].label);
        XmStringFree(actions[i].shortcut);
    }
    XtFree((char *)actions);
}

static void preferred(Widget child, Dimension *width, Dimension *height)
{
    XtWidgetGeometry wanted;

    wanted.request_mode = 0;
    XtQueryGeometry(child, NULL, &wanted);
    *width = wanted.request_mode & CWWidth ? wanted.width : child->core.width;
    *height = wanted.request_mode & CWHeight ? wanted.height : child->core.height;
}

static void natural(XmToolbarWidget toolbar, Dimension *width, Dimension *height)
{
    Cardinal i;
    Dimension child_width, child_height;
    unsigned long total = 0;

    *height = 1;
    for (i = 0; i < toolbar->toolbar.action_count; ++i) {
        preferred(toolbar->toolbar.actions[i].button, &child_width, &child_height);
        total += child_width;
        if (i)
            total += toolbar->toolbar.spacing;
        if (child_height > *height)
            *height = child_height;
    }
    *width = (Dimension)(total > USHRT_MAX ? USHRT_MAX : total ? total : 1);
}

static Cardinal choose_actions(XmToolbarWidget toolbar, Boolean *chosen,
                               Dimension *action_width,
                               Dimension *action_height)
{
    Cardinal i, selected = 0;
    unsigned long total = 0;
    Dimension width, height;

    for (i = 0; i < toolbar->toolbar.action_count; ++i) {
        preferred(toolbar->toolbar.actions[i].button, &width, &height);
        action_width[i] = width;
        action_height[i] = height;
        if (!toolbar->toolbar.actions[i].overflowable) {
            chosen[i] = True;
            total += width + (selected ? toolbar->toolbar.spacing : 0);
            ++selected;
        }
    }
    for (;;) {
        Cardinal candidate = toolbar->toolbar.action_count;
        unsigned char priority = 0;
        for (i = 0; i < toolbar->toolbar.action_count; ++i) {
            if (chosen[i] || !toolbar->toolbar.actions[i].overflowable ||
                (candidate < toolbar->toolbar.action_count &&
                 toolbar->toolbar.actions[i].priority <= priority))
                continue;
            candidate = i;
            priority = toolbar->toolbar.actions[i].priority;
        }
        if (candidate == toolbar->toolbar.action_count)
            break;
        if (total && total + toolbar->toolbar.spacing + action_width[candidate] >
                         toolbar->core.width)
            break;
        if (!total && action_width[candidate] > toolbar->core.width &&
            toolbar->core.width)
            break;
        chosen[candidate] = True;
        total += action_width[candidate] +
                 (selected ? toolbar->toolbar.spacing : 0);
        ++selected;
    }
    return selected;
}

static void layout(XmToolbarWidget toolbar)
{
    Cardinal i, visible, overflow = 0;
    long x = 0;
    Dimension width, height, *action_width, *action_height;
    Boolean *chosen;
    unsigned long total = 0;
    Boolean has_overflow = False;

    if (toolbar->toolbar.initializing || toolbar->toolbar.laying_out)
        return;
    toolbar->toolbar.laying_out = True;
    chosen = toolbar->toolbar.action_count
                 ? XtCalloc(toolbar->toolbar.action_count, sizeof(Boolean)) : NULL;
    action_width = toolbar->toolbar.action_count
                       ? (Dimension *)XtCalloc(toolbar->toolbar.action_count,
                                               sizeof(Dimension)) : NULL;
    action_height = toolbar->toolbar.action_count
                        ? (Dimension *)XtCalloc(toolbar->toolbar.action_count,
                                                sizeof(Dimension)) : NULL;
    visible = choose_actions(toolbar, chosen, action_width, action_height);
    for (i = 0; i < toolbar->toolbar.action_count; ++i)
        if (toolbar->toolbar.actions[i].overflowable && !chosen[i]) {
            has_overflow = True;
            ++overflow;
        }
    if (has_overflow) {
        preferred(toolbar->toolbar.overflow_button, &width, &height);
        total = 0;
        for (i = 0; i < toolbar->toolbar.action_count; ++i)
            if (chosen[i])
                total += action_width[i] + (total ? toolbar->toolbar.spacing : 0);
        while (visible && total + toolbar->toolbar.spacing + width >
                              toolbar->core.width) {
            Cardinal candidate = toolbar->toolbar.action_count;
            unsigned char priority = UCHAR_MAX;
            for (i = 0; i < toolbar->toolbar.action_count; ++i)
                if (chosen[i] && toolbar->toolbar.actions[i].overflowable &&
                    toolbar->toolbar.actions[i].priority <= priority) {
                    candidate = i;
                    priority = toolbar->toolbar.actions[i].priority;
                }
            if (candidate == toolbar->toolbar.action_count)
                break;
            chosen[candidate] = False;
            --visible;
            total = 0;
            for (i = 0; i < toolbar->toolbar.action_count; ++i)
                if (chosen[i])
                    total += action_width[i] + (total ? toolbar->toolbar.spacing : 0);
        }
        XtManageChild(toolbar->toolbar.overflow_button);
        overflow = 0;
        for (i = 0; i < toolbar->toolbar.action_count; ++i)
            if (toolbar->toolbar.actions[i].overflowable && !chosen[i])
                ++overflow;
    } else {
        XtUnmanageChild(toolbar->toolbar.overflow_button);
        for (i = 0; i < toolbar->toolbar.action_count; ++i)
            XtUnmanageChild(toolbar->toolbar.actions[i].menu_button);
    }
    for (i = 0; i < toolbar->toolbar.action_count; ++i) {
        XmToolbarStoredAction *action = &toolbar->toolbar.actions[i];
        action->visible = chosen && chosen[i];
        XtSetMappedWhenManaged(action->button, action->visible);
        if (has_overflow && action->overflowable && !action->visible)
            XtManageChild(action->menu_button);
        else
            XtUnmanageChild(action->menu_button);
        if (action->visible) {
            Position placement = x > SHRT_MAX ? SHRT_MAX : (Position)x;
            XmeConfigureObject(action->button, placement, 0, action_width[i],
                               action_height[i], 0);
            if (x <= SHRT_MAX)
                x += action_width[i] + toolbar->toolbar.spacing;
        }
    }
    if (has_overflow) {
        preferred(toolbar->toolbar.overflow_button, &width, &height);
        {
            Position placement = x > SHRT_MAX ? SHRT_MAX : (Position)x;
            XmeConfigureObject(toolbar->toolbar.overflow_button, placement, 0,
                               width, height, 0);
        }
    }
    toolbar->toolbar.visible_count = visible;
    toolbar->toolbar.overflow_count = overflow;
    XtFree((char *)action_height);
    XtFree((char *)action_width);
    XtFree((char *)chosen);
    toolbar->toolbar.laying_out = False;
}

static void negotiate(XmToolbarWidget toolbar)
{
    Dimension width, height, reply_width, reply_height;
    XtGeometryResult result;

    if (toolbar->toolbar.initializing || toolbar->toolbar.laying_out)
        return;
    natural(toolbar, &width, &height);
    result = XtMakeResizeRequest((Widget)toolbar, width, height, &reply_width,
                                 &reply_height);
    if (result == XtGeometryAlmost)
        XtMakeResizeRequest((Widget)toolbar, reply_width, reply_height, NULL, NULL);
    layout(toolbar);
}

static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count)
{
    XmToolbarWidget toolbar = (XmToolbarWidget)widget;
    XmString label;
    (void)request;
    (void)args;
    (void)count;
    toolbar->toolbar.initializing = True;
    toolbar->toolbar.laying_out = False;
    toolbar->toolbar.overflow_menu = XmCreatePulldownMenu(widget,
                                                          (char *)"overflowMenu",
                                                          NULL, 0);
    label = XmStringCreateLocalized(toolbar->toolbar.overflow_label);
    toolbar->toolbar.overflow_button = XtVaCreateWidget(
        "overflow", xmCascadeButtonWidgetClass, widget, XmNlabelString, label,
        XmNsubMenuId, toolbar->toolbar.overflow_menu, XmNborderWidth, 0, NULL);
    XmStringFree(label);
    toolbar->toolbar.initializing = False;
}

static void Destroy(Widget widget)
{
    XmToolbarWidget toolbar = (XmToolbarWidget)widget;
    /* Xt is already destroying every internal child as part of the parent
     * teardown; only release the copied model here. */
    free_actions(toolbar, False);
}

static void Resize(Widget widget) { layout((XmToolbarWidget)widget); }
static void ChangeManaged(Widget widget) { negotiate((XmToolbarWidget)widget); }

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count)
{
    XmToolbarWidget old = (XmToolbarWidget)old_widget;
    XmToolbarWidget toolbar = (XmToolbarWidget)widget;
    (void)request;
    (void)args;
    (void)count;
    if (old->toolbar.spacing != toolbar->toolbar.spacing ||
        old->toolbar.overflow_label != toolbar->toolbar.overflow_label) {
        layout(toolbar);
        return True;
    }
    return False;
}

static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request,
                                        XtWidgetGeometry *reply)
{
    XmToolbarWidget toolbar = (XmToolbarWidget)XtParent(child);
    if (request->request_mode & (CWX | CWY | CWBorderWidth | CWSibling | CWStackMode))
        return XtGeometryNo;
    if (!toolbar->toolbar.laying_out && !(request->request_mode & XtCWQueryOnly))
        negotiate(toolbar);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = child->core.width;
    reply->height = child->core.height;
    return XtGeometryAlmost;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply)
{
    XmToolbarWidget toolbar = (XmToolbarWidget)widget;
    natural(toolbar, &reply->width, &reply->height);
    reply->request_mode = CWWidth | CWHeight;
    if (request && request->width == reply->width && request->height == reply->height)
        return XtGeometryYes;
    return XtGeometryAlmost;
}

static void activate(Widget child, XtPointer client, XtPointer call)
{
    XmToolbarWidget toolbar = (XmToolbarWidget)client;
    Cardinal i;
    XmToolbarCallbackStruct callback;
    for (i = 0; i < toolbar->toolbar.action_count; ++i) {
        XmToolbarStoredAction *action = &toolbar->toolbar.actions[i];
        if (action->button == child || action->menu_button == child) {
            callback.reason = XmTOOLBAR_REASON_INVOKED;
            callback.event = call ? ((XmPushButtonCallbackStruct *)call)->event : NULL;
            callback.id = action->id;
            callback.user_data = action->user_data;
            XtCallCallbacks((Widget)toolbar, XmNtoolbarActivateCallback, &callback);
            return;
        }
    }
}

Widget XmCreateToolbar(Widget parent, char *name, ArgList args, Cardinal count)
{
    return XtCreateWidget(name, xmToolbarWidgetClass, parent, args, count);
}

Widget XmVaCreateToolbar(Widget parent, char *name, ...)
{
    Widget widget;
    va_list values;
    int count;
    Va_start(values, name);
    count = XmeCountVaListSimple(values);
    va_end(values);
    Va_start(values, name);
    widget = XmeVLCreateWidget(name, xmToolbarWidgetClass, parent, False,
                               values, count);
    va_end(values);
    return widget;
}

Widget XmVaCreateManagedToolbar(Widget parent, char *name, ...)
{
    Widget widget;
    va_list values;
    int count;
    Va_start(values, name);
    count = XmeCountVaListSimple(values);
    va_end(values);
    Va_start(values, name);
    widget = XmeVLCreateWidget(name, xmToolbarWidgetClass, parent, True,
                               values, count);
    va_end(values);
    return widget;
}

void XmToolbarSetActions(Widget widget, const XmToolbarAction *actions,
                         Cardinal count)
{
    XmToolbarWidget toolbar;
    Cardinal i;
    if (!widget || !XmIsToolbar(widget) || (count && !actions))
        return;
    toolbar = (XmToolbarWidget)widget;
    toolbar->toolbar.laying_out = True;
    free_actions(toolbar, True);
    if (count)
        toolbar->toolbar.actions = (XmToolbarStoredAction *)XtCalloc(
            count, sizeof(*toolbar->toolbar.actions));
    for (i = 0; i < count; ++i) {
        XmString label = action_label(&actions[i]);
        XmToolbarStoredAction *stored = &toolbar->toolbar.actions[i];
        stored->id = XtNewString(actions[i].id ? actions[i].id : "");
        stored->label = string_copy(actions[i].label);
        stored->shortcut = string_copy(actions[i].shortcut);
        stored->overflowable = actions[i].overflowable;
        stored->priority = actions[i].priority;
        stored->user_data = actions[i].user_data;
        stored->button = XtVaCreateManagedWidget(
            "action", xmPushButtonWidgetClass, widget, XmNlabelString, label,
            XmNborderWidth, 0, NULL);
        stored->menu_button = XtVaCreateWidget(
            "overflowAction", xmPushButtonWidgetClass,
            toolbar->toolbar.overflow_menu, XmNlabelString, label,
            XmNborderWidth, 0, NULL);
        XtAddCallback(stored->button, XmNactivateCallback, activate, toolbar);
        XtAddCallback(stored->menu_button, XmNactivateCallback, activate, toolbar);
        XmStringFree(label);
    }
    toolbar->toolbar.action_count = count;
    toolbar->toolbar.laying_out = False;
    layout(toolbar);
    negotiate(toolbar);
}

Cardinal XmToolbarGetActionCount(Widget widget)
{
    return widget && XmIsToolbar(widget)
               ? ((XmToolbarWidget)widget)->toolbar.action_count : 0;
}

Cardinal XmToolbarGetVisibleActionCount(Widget widget)
{
    return widget && XmIsToolbar(widget)
               ? ((XmToolbarWidget)widget)->toolbar.visible_count : 0;
}

Cardinal XmToolbarGetOverflowCount(Widget widget)
{
    return widget && XmIsToolbar(widget)
               ? ((XmToolbarWidget)widget)->toolbar.overflow_count : 0;
}

void XmToolbarInvoke(Widget widget, Cardinal index, XEvent *event)
{
    XmToolbarWidget toolbar;
    XmToolbarCallbackStruct callback;
    if (!widget || !XmIsToolbar(widget))
        return;
    toolbar = (XmToolbarWidget)widget;
    if (index >= toolbar->toolbar.action_count)
        return;
    callback.reason = XmTOOLBAR_REASON_INVOKED;
    callback.event = event;
    callback.id = toolbar->toolbar.actions[index].id;
    callback.user_data = toolbar->toolbar.actions[index].user_data;
    XtCallCallbacks(widget, XmNtoolbarActivateCallback, &callback);
}
