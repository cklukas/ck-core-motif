#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <Xm/AutoSuggestBoxP.h>
#include <Xm/List.h>
#include <Xm/SearchBox.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
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
static void query_changed(Widget, XtPointer, XtPointer);
static void search_committed(Widget, XtPointer, XtPointer);
static void list_selected(Widget, XtPointer, XtPointer);
static void search_events(Widget, XtPointer, XEvent *, Boolean *);
static void list_events(Widget, XtPointer, XEvent *, Boolean *);

#define Offset(field) XtOffsetOf(XmAutoSuggestBoxRec, auto_suggest.field)
static XtResource resources[] = {
    {XmNautoSuggestNoResultsLabel, XmCAutoSuggestNoResultsLabel, XmRString,
     sizeof(String), Offset(no_results_label), XmRImmediate,
     (XtPointer)"No suggestions"},
    {XmNautoSuggestQueryChangedCallback, XmCAutoSuggestQueryChangedCallback,
     XmRCallback, sizeof(XtCallbackList), Offset(query_changed_callback),
     XmRPointer, NULL},
    {XmNautoSuggestCommittedCallback, XmCAutoSuggestCommittedCallback,
     XmRCallback, sizeof(XtCallbackList), Offset(committed_callback),
     XmRPointer, NULL},
    {XmNautoSuggestDismissedCallback, XmCAutoSuggestDismissedCallback,
     XmRCallback, sizeof(XtCallbackList), Offset(dismissed_callback),
     XmRPointer, NULL}
};

externaldef(xmautosuggestboxclassrec) XmAutoSuggestBoxClassRec
    xmAutoSuggestBoxClassRec = {
        {.superclass = (WidgetClass)&xmManagerClassRec,
         .class_name = "XmAutoSuggestBox",
         .widget_size = sizeof(XmAutoSuggestBoxRec), .initialize = Initialize,
         .realize = XtInheritRealize, .resources = resources,
         .num_resources = XtNumber(resources), .compress_motion = True,
         .compress_exposure = XtExposeCompressMultiple,
         .compress_enterleave = True, .destroy = Destroy, .resize = Resize,
         .set_values = SetValues,
         .set_values_almost = XtInheritSetValuesAlmost, .version = XtVersion,
         .tm_table = XtInheritTranslations, .query_geometry = QueryGeometry},
        {.geometry_manager = GeometryManager, .change_managed = ChangeManaged,
         .insert_child = XtInheritInsertChild,
         .delete_child = XtInheritDeleteChild},
        {.constraint_size = sizeof(XmManagerConstraintRec)},
        {.translations = XtInheritTranslations,
         .parent_process = XmInheritParentProcess},
        {NULL}};

externaldef(xmautosuggestboxwidgetclass) WidgetClass
    xmAutoSuggestBoxWidgetClass = (WidgetClass)&xmAutoSuggestBoxClassRec;

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
    XmString result = XmStringCreateLocalized(text);
    XtFree(text);
    return result;
}

static void free_items(XmAutoSuggestBoxWidget box)
{
    XmAutoSuggestStoredItem *items = box->auto_suggest.items;
    Cardinal count = box->auto_suggest.item_count;
    Cardinal i;
    box->auto_suggest.items = NULL;
    box->auto_suggest.item_count = 0;
    for (i = 0; i < count; ++i) {
        XtFree(items[i].id);
        XmStringFree(items[i].label);
    }
    XtFree((char *)items);
}

static Widget shell_ancestor(Widget widget)
{
    while (widget && !XtIsShell(widget))
        widget = XtParent(widget);
    return widget;
}

static void position_popup(XmAutoSuggestBoxWidget box)
{
    Position x, y;
    Dimension width, height;
    Dimension popup_width, popup_height;
    Widget shell = box->auto_suggest.popup_shell;
    Screen *screen;
    int max_x, max_y;
    if (!shell || !XtIsRealized(box->auto_suggest.search))
        return;
    XtTranslateCoords(box->auto_suggest.search, 0, 0, &x, &y);
    XtVaGetValues(box->auto_suggest.search, XmNwidth, &width,
                  XmNheight, &height, NULL);
    XtVaGetValues(shell, XmNwidth, &popup_width, XmNheight, &popup_height,
                  NULL);
    x += (Position)((int)width - (int)popup_width) / 2;
    y += (Position)height;
    screen = XtScreen(shell);
    max_x = WidthOfScreen(screen) - (int)popup_width;
    max_y = HeightOfScreen(screen) - (int)popup_height;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (max_x >= 0 && x > max_x)
        x = (Position)max_x;
    if (max_y >= 0 && y > max_y)
        y = (Position)max_y;
    XtVaSetValues(shell, XtNx, x, XtNy, y, NULL);
}

static void create_popup(XmAutoSuggestBoxWidget box)
{
    Widget parent_shell = shell_ancestor((Widget)box);
    Arg args[5];
    Cardinal count = 0;
    if (box->auto_suggest.popup_shell || !parent_shell)
        return;
    XtSetArg(args[count], XmNoverrideRedirect, True); ++count;
    XtSetArg(args[count], XmNmappedWhenManaged, False); ++count;
    XtSetArg(args[count], XmNsaveUnder, True); ++count;
    XtSetArg(args[count], XmNtransientFor, XtWindow(parent_shell)); ++count;
    box->auto_suggest.popup_shell = XtCreatePopupShell(
        "suggestionsShell", overrideShellWidgetClass, parent_shell, args,
        count);
    box->auto_suggest.list = XmCreateList(box->auto_suggest.popup_shell,
                                         (char *)"suggestions", NULL, 0);
    XtVaSetValues(box->auto_suggest.list, XmNselectionPolicy, XmSINGLE_SELECT,
                  XmNvisibleItemCount, 5, NULL);
    XtAddCallback(box->auto_suggest.list, XmNdefaultActionCallback,
                  list_selected, box);
    XtAddEventHandler(box->auto_suggest.list, KeyPressMask, True, list_events,
                      box);
    XtManageChild(box->auto_suggest.list);
    XtRealizeWidget(box->auto_suggest.popup_shell);
}

static void emit_query(XmAutoSuggestBoxWidget box, XEvent *event)
{
    XmAutoSuggestCallbackStruct callback;
    char *query = XmSearchBoxGetString(box->auto_suggest.search);
    callback.reason = XmAUTO_SUGGEST_REASON_QUERY_CHANGED;
    callback.event = event;
    callback.query = query ? query : "";
    callback.generation = box->auto_suggest.generation;
    callback.id = NULL;
    callback.user_data = NULL;
    XtCallCallbackList((Widget)box, box->auto_suggest.query_changed_callback,
                       &callback);
    XtFree(query);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)widget;
    (void)request;
    (void)args;
    (void)count;
    box->auto_suggest.initializing = True;
    box->auto_suggest.generation = 0;
    box->auto_suggest.visible = False;
    box->auto_suggest.no_results = False;
    box->auto_suggest.search = XmVaCreateManagedSearchBox(
        widget, (char *)"search", XmNtraversalOn, True, NULL);
    XtAddCallback(box->auto_suggest.search, XmNsearchQueryChangedCallback,
                  query_changed, box);
    XtAddCallback(box->auto_suggest.search, XmNsearchCommittedCallback,
                  search_committed, box);
    XtAddEventHandler(box->auto_suggest.search, KeyPressMask, True,
                      search_events, box);
    box->auto_suggest.initializing = False;
}

static void Destroy(Widget widget)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)widget;
    if (box->auto_suggest.search &&
        !((CoreWidget)box->auto_suggest.search)->core.being_destroyed) {
        XtRemoveCallback(box->auto_suggest.search,
                         XmNsearchQueryChangedCallback, query_changed, box);
        XtRemoveCallback(box->auto_suggest.search,
                         XmNsearchCommittedCallback, search_committed, box);
        XtRemoveEventHandler(box->auto_suggest.search, KeyPressMask, True,
                             search_events, box);
    }
    free_items(box);
    if (box->auto_suggest.popup_shell &&
        !((CoreWidget)box->auto_suggest.popup_shell)->core.being_destroyed &&
        !((CoreWidget)shell_ancestor(widget))->core.being_destroyed)
        XtDestroyWidget(box->auto_suggest.popup_shell);
}

static void layout(XmAutoSuggestBoxWidget box)
{
    if (box->auto_suggest.initializing || box->auto_suggest.laying_out)
        return;
    box->auto_suggest.laying_out = True;
    XmeConfigureObject(box->auto_suggest.search, 0, 0, box->core.width,
                       box->core.height ? box->core.height : 1, 0);
    box->auto_suggest.laying_out = False;
}

static void Resize(Widget widget) { layout((XmAutoSuggestBoxWidget)widget); }
static void ChangeManaged(Widget widget) { layout((XmAutoSuggestBoxWidget)widget); }

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count)
{
    (void)old_widget;
    (void)request;
    (void)widget;
    (void)args;
    (void)count;
    return False;
}

static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request,
                                        XtWidgetGeometry *reply)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)XtParent(child);
    if (request->request_mode & (CWX | CWY | CWBorderWidth | CWSibling | CWStackMode))
        return XtGeometryNo;
    if (!box->auto_suggest.laying_out && !(request->request_mode & XtCWQueryOnly))
        layout(box);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = child->core.width;
    reply->height = child->core.height;
    return XtGeometryAlmost;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)widget;
    XtWidgetGeometry wanted;
    wanted.request_mode = 0;
    XtQueryGeometry(box->auto_suggest.search, NULL, &wanted);
    reply->width = wanted.request_mode & CWWidth ? wanted.width : 1;
    reply->height = wanted.request_mode & CWHeight ? wanted.height : 1;
    reply->request_mode = CWWidth | CWHeight;
    if (request && request->width == reply->width && request->height == reply->height)
        return XtGeometryYes;
    return XtGeometryAlmost;
}

static void query_changed(Widget widget, XtPointer client, XtPointer call)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)client;
    XmSearchBoxCallbackStruct *search_callback =
        (XmSearchBoxCallbackStruct *)call;
    (void)widget;
    ++box->auto_suggest.generation;
    if (!box->auto_suggest.committing)
        XmAutoSuggestBoxDismiss((Widget)box,
                                search_callback ? search_callback->event : NULL);
    emit_query(box, search_callback ? search_callback->event : NULL);
}

static void search_committed(Widget widget, XtPointer client, XtPointer call)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)client;
    XmSearchBoxCallbackStruct *search_callback =
        (XmSearchBoxCallbackStruct *)call;
    int *positions = NULL;
    int count = 0;
    (void)widget;
    (void)call;
    if (XmListGetSelectedPos(box->auto_suggest.list, &positions, &count) &&
        count == 1)
        XmAutoSuggestBoxCommit((Widget)box, (Cardinal)(positions[0] - 1),
                               search_callback ? search_callback->event : NULL);
    XtFree((char *)positions);
}

static void list_selected(Widget widget, XtPointer client, XtPointer call)
{
    XmListCallbackStruct *selection = (XmListCallbackStruct *)call;
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)client;
    (void)widget;
    if (selection && selection->item_position > 0)
        XmAutoSuggestBoxCommit((Widget)box,
                               (Cardinal)(selection->item_position - 1),
                               selection->event);
}

static void search_events(Widget widget, XtPointer client, XEvent *event,
                          Boolean *dispatch)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)client;
    KeySym key;
    char input[2];
    (void)widget;
    if (event->type != KeyPress)
        return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_Escape) {
        XmAutoSuggestBoxDismiss((Widget)box, event);
        if (dispatch)
            *dispatch = False;
    } else if (key == XK_Down && box->auto_suggest.visible) {
        XmProcessTraversal(box->auto_suggest.list, XmTRAVERSE_CURRENT);
        if (dispatch)
            *dispatch = False;
    }
}

static void list_events(Widget widget, XtPointer client, XEvent *event,
                        Boolean *dispatch)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)client;
    KeySym key;
    char input[2];
    (void)widget;
    if (event->type != KeyPress)
        return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_Escape) {
        XmAutoSuggestBoxDismiss((Widget)box, event);
        XmProcessTraversal(box->auto_suggest.search, XmTRAVERSE_CURRENT);
        if (dispatch)
            *dispatch = False;
    }
}

Widget XmCreateAutoSuggestBox(Widget parent, char *name, ArgList args,
                              Cardinal count)
{
    return XtCreateWidget(name, xmAutoSuggestBoxWidgetClass, parent, args,
                          count);
}

Widget XmVaCreateAutoSuggestBox(Widget parent, char *name, ...)
{
    Widget widget;
    va_list values;
    int count;
    Va_start(values, name);
    count = XmeCountVaListSimple(values);
    va_end(values);
    Va_start(values, name);
    widget = XmeVLCreateWidget(name, xmAutoSuggestBoxWidgetClass, parent, False,
                               values, count);
    va_end(values);
    return widget;
}

Widget XmVaCreateManagedAutoSuggestBox(Widget parent, char *name, ...)
{
    Widget widget;
    va_list values;
    int count;
    Va_start(values, name);
    count = XmeCountVaListSimple(values);
    va_end(values);
    Va_start(values, name);
    widget = XmeVLCreateWidget(name, xmAutoSuggestBoxWidgetClass, parent, True,
                               values, count);
    va_end(values);
    return widget;
}

Boolean XmAutoSuggestBoxSetSuggestions(Widget widget,
                                       const XmAutoSuggestItem *items,
                                       Cardinal count,
                                       unsigned long generation)
{
    XmAutoSuggestBoxWidget box;
    Cardinal i;
    if (!widget || !XmIsAutoSuggestBox(widget) || (count && !items))
        return False;
    box = (XmAutoSuggestBoxWidget)widget;
    if (generation != box->auto_suggest.generation)
        return False;
    create_popup(box);
    if (!box->auto_suggest.list)
        return False;
    free_items(box);
    XmListDeleteAllItems(box->auto_suggest.list);
    if (count)
        box->auto_suggest.items = (XmAutoSuggestStoredItem *)XtCalloc(
            count, sizeof(*box->auto_suggest.items));
    box->auto_suggest.item_count = count;
    box->auto_suggest.no_results = count == 0;
    if (count) {
        for (i = 0; i < count; ++i) {
            XmString label = string_copy(items[i].label);
            box->auto_suggest.items[i].id = XtNewString(items[i].id ? items[i].id : "");
            box->auto_suggest.items[i].label = string_copy(items[i].label);
            box->auto_suggest.items[i].user_data = items[i].user_data;
            XmListAddItem(box->auto_suggest.list, label, 0);
            XmStringFree(label);
        }
    } else {
        XmString label = XmStringCreateLocalized(box->auto_suggest.no_results_label);
        XmListAddItem(box->auto_suggest.list, label, 0);
        XmStringFree(label);
    }
    XmListDeselectAllItems(box->auto_suggest.list);
    if (count || box->auto_suggest.no_results)
        XmAutoSuggestBoxShow(widget);
    return True;
}

Cardinal XmAutoSuggestBoxGetSuggestionCount(Widget widget)
{
    return widget && XmIsAutoSuggestBox(widget)
               ? ((XmAutoSuggestBoxWidget)widget)->auto_suggest.item_count : 0;
}

unsigned long XmAutoSuggestBoxGetGeneration(Widget widget)
{
    return widget && XmIsAutoSuggestBox(widget)
               ? ((XmAutoSuggestBoxWidget)widget)->auto_suggest.generation : 0;
}

void XmAutoSuggestBoxShow(Widget widget)
{
    XmAutoSuggestBoxWidget box;
    if (!widget || !XmIsAutoSuggestBox(widget))
        return;
    box = (XmAutoSuggestBoxWidget)widget;
    if (!XtIsRealized(box->auto_suggest.search))
        return;
    create_popup(box);
    if (!box->auto_suggest.popup_shell)
        return;
    position_popup(box);
    XtPopup(box->auto_suggest.popup_shell, XtGrabNone);
    box->auto_suggest.visible = True;
}

void XmAutoSuggestBoxDismiss(Widget widget, XEvent *event)
{
    XmAutoSuggestBoxWidget box;
    XmAutoSuggestCallbackStruct callback;
    char *query;
    if (!widget || !XmIsAutoSuggestBox(widget))
        return;
    box = (XmAutoSuggestBoxWidget)widget;
    if (!box->auto_suggest.visible)
        return;
    box->auto_suggest.visible = False;
    if (box->auto_suggest.popup_shell)
        XtPopdown(box->auto_suggest.popup_shell);
    query = XmSearchBoxGetString(box->auto_suggest.search);
    callback.reason = XmAUTO_SUGGEST_REASON_DISMISSED;
    callback.event = event;
    callback.query = query ? query : "";
    callback.generation = box->auto_suggest.generation;
    callback.id = NULL;
    callback.user_data = NULL;
    XtCallCallbackList(widget, box->auto_suggest.dismissed_callback, &callback);
    XtFree(query);
}

void XmAutoSuggestBoxCommit(Widget widget, Cardinal index, XEvent *event)
{
    XmAutoSuggestBoxWidget box;
    XmAutoSuggestCallbackStruct callback;
    char *query;
    char *label;
    char *id;
    XtPointer user_data;
    if (!widget || !XmIsAutoSuggestBox(widget))
        return;
    box = (XmAutoSuggestBoxWidget)widget;
    if (index >= box->auto_suggest.item_count)
        return;
    id = XtNewString(box->auto_suggest.items[index].id);
    user_data = box->auto_suggest.items[index].user_data;
    label = text_copy(box->auto_suggest.items[index].label);
    box->auto_suggest.committing = True;
    XmSearchBoxSetString(box->auto_suggest.search, label);
    box->auto_suggest.committing = False;
    XtFree(label);
    query = XmSearchBoxGetString(box->auto_suggest.search);
    callback.reason = XmAUTO_SUGGEST_REASON_COMMITTED;
    callback.event = event;
    callback.query = query ? query : "";
    callback.generation = box->auto_suggest.generation;
    callback.id = id;
    callback.user_data = user_data;
    XtCallCallbackList(widget, box->auto_suggest.committed_callback, &callback);
    XtFree(id);
    XtFree(query);
    XmAutoSuggestBoxDismiss(widget, event);
    XmProcessTraversal(box->auto_suggest.search, XmTRAVERSE_CURRENT);
}

Widget XmAutoSuggestBoxGetSearchField(Widget widget)
{
    return widget && XmIsAutoSuggestBox(widget)
               ? ((XmAutoSuggestBoxWidget)widget)->auto_suggest.search : NULL;
}
