#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Xm/RowActionsP.h>
#include <Xm/PushB.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <X11/keysym.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Resize(Widget);
static void ChangeManaged(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void activate(Widget, XtPointer, XtPointer);
static void events(Widget, XtPointer, XEvent *, Boolean *);
#define Offset(field) XtOffsetOf(XmRowActionsRec, actions.field)
static XtResource resources[] = {
    {XmNrowActionsSide, XmCRowActionsSide, XmRUnsignedChar, sizeof(unsigned char), Offset(side), XmRImmediate, (XtPointer)XmROW_ACTIONS_SIDE_END},
    {XmNrowActionsInvokeCallback, XmCRowActionsInvokeCallback, XmRCallback, sizeof(XtCallbackList), Offset(invoke_callback), XmRPointer, NULL}
};
externaldef(xmrowactionsclassrec) XmRowActionsClassRec xmRowActionsClassRec = {
    {.superclass = (WidgetClass)&xmManagerClassRec, .class_name = "XmRowActions",
     .widget_size = sizeof(XmRowActionsRec), .initialize = Initialize,
     .realize = XtInheritRealize, .resources = resources,
     .num_resources = XtNumber(resources), .compress_motion = True,
     .compress_exposure = XtExposeCompressMultiple, .compress_enterleave = True,
     .destroy = Destroy, .resize = Resize, .set_values = SetValues,
     .set_values_almost = XtInheritSetValuesAlmost, .version = XtVersion,
     .tm_table = XtInheritTranslations, .query_geometry = QueryGeometry},
    {.geometry_manager = GeometryManager, .change_managed = ChangeManaged,
     .insert_child = XtInheritInsertChild, .delete_child = XtInheritDeleteChild},
    {.constraint_size = sizeof(XmManagerConstraintRec)},
    {.translations = XtInheritTranslations, .parent_process = XmInheritParentProcess}, {NULL}};
externaldef(xmrowactionswidgetclass) WidgetClass xmRowActionsWidgetClass =
    (WidgetClass)&xmRowActionsClassRec;

static char *text_copy(XmString value) {
    char *text = value ? XmStringUnparse(value, NULL, XmCHARSET_TEXT,
                                          XmCHARSET_TEXT, NULL, 0,
                                          XmOUTPUT_ALL) : NULL;
    return text ? text : XtNewString("");
}
static XmString string_copy(XmString value) {
    char *text = text_copy(value); XmString result;
    if (!text) return NULL;
    result = XmStringCreateLocalized(text);
    XtFree(text); return result;
}
static char *button_text(XmString label, XmString shortcut) {
    char *left = text_copy(label), *right = text_copy(shortcut), *combined;
    size_t left_length, right_length, length;
    if (!left || !right) { XtFree(left); XtFree(right); return NULL; }
    left_length = strlen(left); right_length = strlen(right);
    if (right[0]) {
        if (left_length > SIZE_MAX - 3 || right_length > SIZE_MAX - 3 - left_length) {
            XtFree(left); XtFree(right); return NULL;
        }
        length = left_length + right_length + 3;
    } else {
        if (left_length == SIZE_MAX) { XtFree(left); XtFree(right); return NULL; }
        length = left_length + 1;
    }
    combined = XtMalloc(length);
    if (!combined) { XtFree(left); XtFree(right); return NULL; }
    memcpy(combined, left, left_length);
    if (right[0]) {
        memcpy(combined + left_length, "  ", 2);
        memcpy(combined + left_length + 2, right, right_length);
    }
    combined[length - 1] = '\0';
    XtFree(left); XtFree(right); return combined;
}
static void free_items(XmRowActionsWidget actions, Boolean destroy_buttons) {
    Cardinal i;
    for (i = 0; i < actions->actions.item_count; ++i) {
        if (destroy_buttons && actions->actions.items[i].button)
            XtDestroyWidget(actions->actions.items[i].button);
        XtFree(actions->actions.items[i].id);
        XmStringFree(actions->actions.items[i].label);
        XmStringFree(actions->actions.items[i].shortcut);
    }
    XtFree((char *)actions->actions.items); actions->actions.items = NULL;
    actions->actions.item_count = 0;
}
static void preferred(Widget child, Dimension *width, Dimension *height) {
    XtWidgetGeometry wanted; wanted.request_mode = 0; XtQueryGeometry(child, NULL, &wanted);
    *width = wanted.request_mode & CWWidth ? wanted.width : child->core.width;
    *height = wanted.request_mode & CWHeight ? wanted.height : child->core.height;
}
static void layout(XmRowActionsWidget actions) {
    Cardinal i; long x = 0; Dimension width, height;
    if (actions->actions.initializing || actions->actions.laying_out) return;
    actions->actions.laying_out = True;
    for (i = 0; i < actions->actions.item_count; ++i) {
        preferred(actions->actions.items[i].button, &width, &height);
        XtSetMappedWhenManaged(actions->actions.items[i].button,
                               actions->actions.revealed);
        if (actions->actions.revealed) {
            Position placement = x > SHRT_MAX ? SHRT_MAX : (Position)x;
            XmeConfigureObject(actions->actions.items[i].button, placement, 0, width,
                               height, 0); x += width;
        }
    }
    actions->actions.laying_out = False;
}
static void natural(XmRowActionsWidget actions, Dimension *width, Dimension *height) {
    Cardinal i; Dimension child_width, child_height; unsigned long total = 0;
    *height = 1;
    for (i = 0; i < actions->actions.item_count; ++i) {
        preferred(actions->actions.items[i].button, &child_width, &child_height);
        if (total > (unsigned long)USHRT_MAX - child_width)
            total = USHRT_MAX;
        else
            total += child_width;
        if (child_height > *height) *height = child_height;
    }
    *width = (Dimension)(total > USHRT_MAX ? USHRT_MAX : total ? total : 1);
}
static void negotiate(XmRowActionsWidget actions) {
    Dimension width, height, reply_width, reply_height; XtGeometryResult result;
    if (actions->actions.initializing || actions->actions.laying_out) return;
    natural(actions, &width, &height);
    result = XtMakeResizeRequest((Widget)actions, width, height,
                                 &reply_width, &reply_height);
    if (result == XtGeometryAlmost)
        XtMakeResizeRequest((Widget)actions, reply_width, reply_height, NULL, NULL);
    layout(actions);
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmRowActionsWidget actions = (XmRowActionsWidget)widget;
    (void)request; (void)args; (void)count;
    actions->actions.initializing = False; actions->actions.revealed = False;
    XtAddEventHandler(widget, KeyPressMask, True, events, NULL);
}
static void Destroy(Widget widget) {
    XmRowActionsWidget actions = (XmRowActionsWidget)widget;
    XtRemoveEventHandler(widget, KeyPressMask, True, events, NULL); free_items(actions, False);
}
static void Resize(Widget widget) { layout((XmRowActionsWidget)widget); }
static void ChangeManaged(Widget widget) { negotiate((XmRowActionsWidget)widget); }
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmRowActionsWidget old = (XmRowActionsWidget)old_widget;
    XmRowActionsWidget actions = (XmRowActionsWidget)widget;
    (void)request; (void)args; (void)count;
    if (old->actions.side != actions->actions.side) { layout(actions); return True; }
    return False;
}
static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request,
                                        XtWidgetGeometry *reply) {
    XmRowActionsWidget actions = (XmRowActionsWidget)XtParent(child);
    if (request->request_mode & (CWX | CWY | CWBorderWidth | CWSibling | CWStackMode))
        return XtGeometryNo;
    if (!actions->actions.laying_out && !(request->request_mode & XtCWQueryOnly))
        negotiate(actions);
    reply->request_mode = CWWidth | CWHeight; reply->width = child->core.width;
    reply->height = child->core.height; return XtGeometryAlmost;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *intended,
                                      XtWidgetGeometry *reply) {
    XmRowActionsWidget actions = (XmRowActionsWidget)widget;
    natural(actions, &reply->width, &reply->height); reply->request_mode = CWWidth | CWHeight;
    if (intended && intended->width == reply->width && intended->height == reply->height)
        return XtGeometryYes;
    return XtGeometryAlmost;
}
static void activate(Widget button, XtPointer client, XtPointer call) {
    XmRowActionsWidget actions = (XmRowActionsWidget)client; Cardinal i;
    XmRowActionsCallbackStruct callback;
    for (i = 0; i < actions->actions.item_count; ++i)
        if (actions->actions.items[i].button == button) {
            callback.reason = XmROW_ACTIONS_REASON_INVOKED;
            callback.event = call ? ((XmPushButtonCallbackStruct *)call)->event : NULL;
            callback.id = actions->actions.items[i].id;
            callback.user_data = actions->actions.items[i].user_data;
            XtCallCallbacks((Widget)actions, XmNrowActionsInvokeCallback, &callback);
            return;
        }
}
static void events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) {
    KeySym key; char input[2]; XmRowActionsWidget actions = (XmRowActionsWidget)widget;
    (void)data; if (event->type != KeyPress) return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_space || key == XK_Return) {
        XmRowActionsReveal(widget, !actions->actions.revealed); if (dispatch) *dispatch = False;
    }
}
Widget XmCreateRowActions(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmRowActionsWidgetClass, parent, args, count);
}
Widget XmVaCreateRowActions(Widget parent, char *name, ...) {
    Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v);
    Va_start(v,name); w=XmeVLCreateWidget(name,xmRowActionsWidgetClass,parent,False,v,n);
    va_end(v); return w;
}
Widget XmVaCreateManagedRowActions(Widget parent, char *name, ...) {
    Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v);
    Va_start(v,name); w=XmeVLCreateWidget(name,xmRowActionsWidgetClass,parent,True,v,n);
    va_end(v); return w;
}
void XmRowActionsSetItems(Widget widget, const XmRowActionItem *items, Cardinal count) {
    XmRowActionsWidget actions; Cardinal i;
    if (!widget || !XmIsRowActions(widget) || (count && !items)) return;
    actions = (XmRowActionsWidget)widget;
    /* Child creation can synchronously invoke ChangeManaged.  Keep the
     * aggregate in its initialization phase until every item slot is filled;
     * otherwise geometry negotiation observes a partially initialized array
     * and may query a NULL button. */
    actions->actions.initializing = True;
    free_items(actions, True);
    if (count) {
        if ((size_t)count > SIZE_MAX / sizeof(*actions->actions.items) ||
            (size_t)count > UINT_MAX / sizeof(*actions->actions.items)) {
            XtWarning("XmRowActionsSetItems: item array is too large");
            actions->actions.initializing = False;
            return;
        }
        actions->actions.items = (XmRowActionsStoredItem *)XtCalloc(count, sizeof(*actions->actions.items));
        if (!actions->actions.items) {
            XtWarning("XmRowActionsSetItems: unable to allocate item array");
            actions->actions.initializing = False;
            return;
        }
    }
    for (i = 0; i < count; ++i) {
        char *text; XmString label;
        actions->actions.item_count = i + 1;
        actions->actions.items[i].id = XtNewString(items[i].id ? items[i].id : "");
        actions->actions.items[i].label = string_copy(items[i].label);
        actions->actions.items[i].shortcut = string_copy(items[i].shortcut);
        actions->actions.items[i].user_data = items[i].user_data;
        if (!actions->actions.items[i].id || !actions->actions.items[i].label ||
            !actions->actions.items[i].shortcut) {
            XtWarning("XmRowActionsSetItems: unable to copy item");
            free_items(actions, True); actions->actions.initializing = False; return;
        }
        text = button_text(items[i].label, items[i].shortcut);
        if (!text) { XtWarning("XmRowActionsSetItems: unable to build button label"); free_items(actions, True); actions->actions.initializing = False; return; }
        label = XmStringCreateLocalized(text);
        if (!label) { XtFree(text); XtWarning("XmRowActionsSetItems: unable to create button label"); free_items(actions, True); actions->actions.initializing = False; return; }
        actions->actions.items[i].button = XtVaCreateWidget("action",
            xmPushButtonWidgetClass, widget, XmNlabelString, label,
            XmNborderWidth, 0, NULL); XmStringFree(label); XtFree(text);
        if (!actions->actions.items[i].button) {
            XtWarning("XmRowActionsSetItems: unable to create action button");
            free_items(actions, True); actions->actions.initializing = False; return;
        }
        XtAddCallback(actions->actions.items[i].button, XmNactivateCallback, activate, actions);
    }
    for (i = 0; i < count; ++i)
        XtManageChild(actions->actions.items[i].button);
    actions->actions.initializing = False;
    layout(actions); negotiate(actions);
}
Cardinal XmRowActionsGetItemCount(Widget widget) { return widget && XmIsRowActions(widget) ? ((XmRowActionsWidget)widget)->actions.item_count : 0; }
void XmRowActionsReveal(Widget widget, Boolean revealed) { XmRowActionsWidget actions; if (!widget || !XmIsRowActions(widget)) return; actions = (XmRowActionsWidget)widget; actions->actions.revealed = revealed; layout(actions); }
Boolean XmRowActionsIsRevealed(Widget widget) { return widget && XmIsRowActions(widget) ? ((XmRowActionsWidget)widget)->actions.revealed : False; }
void XmRowActionsInvoke(Widget widget, Cardinal index, XEvent *event) { XmRowActionsWidget actions; XmRowActionsCallbackStruct callback; if (!widget || !XmIsRowActions(widget)) return; actions = (XmRowActionsWidget)widget; if (index >= actions->actions.item_count || !actions->actions.revealed) return; callback.reason = XmROW_ACTIONS_REASON_INVOKED; callback.event = event; callback.id = actions->actions.items[index].id; callback.user_data = actions->actions.items[index].user_data; XtCallCallbacks(widget, XmNrowActionsInvokeCallback, &callback); }
