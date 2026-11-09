#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <Xm/AutoSuggestBoxP.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include "CallbackI.h"
#include "XmI.h"
#include <Xm/SearchBox.h>
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
static void query_changed(Widget, XtPointer, XtPointer);
static void search_committed(Widget, XtPointer, XtPointer);
static void list_selected(Widget, XtPointer, XtPointer);
static void search_events(Widget, XtPointer, XEvent *, Boolean *);
static void list_events(Widget, XtPointer, XEvent *, Boolean *);
static void anchor_events(Widget, XtPointer, XEvent *, Boolean *);
static void layout(XmAutoSuggestBoxWidget);
static void schedule_layout(XmAutoSuggestBoxWidget);
static Boolean dismiss(XmAutoSuggestBoxWidget, XEvent *);
static void cancel_popup(XtPointer, XtIntervalId *);
static void focus_events(Widget, XtPointer, XEvent *, Boolean *);
static void stop_outside_watch(XmAutoSuggestBoxWidget);
static void popup_destroyed(Widget, XtPointer, XtPointer);
static void search_destroyed(Widget, XtPointer, XtPointer);

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

static const char *no_results_text(XmAutoSuggestBoxWidget box)
{
    return box->auto_suggest.no_results_label
               ? box->auto_suggest.no_results_label : "No suggestions";
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

static void free_item_array(XmAutoSuggestStoredItem *items, Cardinal count)
{
    Cardinal i;
    if (items == NULL)
        return;
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

/* Focus may pass through a shell while Xt completes traversal. Resolve the
 * final server focus after dispatch, then consult Motif's logical focus only
 * when that actual focus belongs to a shell. A stale logical focus in another
 * shell must not keep this popup open. */
static void check_focus(XtPointer closure, XtIntervalId *id)
{
    XmAutoSuggestBoxWidget box = closure;
    Window window;
    int revert;
    Widget focus;
    (void)id;
    box->auto_suggest.focus_timer = 0;
    if (!box->auto_suggest.visible || box->core.being_destroyed) return;
    XGetInputFocus(XtDisplay((Widget)box), &window, &revert);
    focus = window != None && window != PointerRoot ?
        XtWindowToWidget(XtDisplay((Widget)box), window) : NULL;
    if (focus && XtIsShell(focus)) {
        Widget logical = XmGetFocusWidget(focus);
        if (logical) focus = logical;
    }
    for (Widget current = focus; current; current = XtParent(current))
        if (current == (Widget)box || current == box->auto_suggest.popup_shell) return;
    dismiss(box, NULL);
}

static void focus_events(Widget widget, XtPointer closure, XEvent *event, Boolean *dispatch)
{
    XmAutoSuggestBoxWidget box = closure;
    (void)widget; (void)dispatch;
    if (event->type != FocusOut || !box->auto_suggest.visible ||
        box->core.being_destroyed || box->auto_suggest.focus_timer) return;
    box->auto_suggest.focus_timer = XtAppAddTimeOut(
        XtWidgetToApplicationContext((Widget)box), 0, check_focus, box);
}

typedef struct _XmAutoSuggestWatch {
    Widget widget;
    XmAutoSuggestBoxWidget owner;
    struct _XmAutoSuggestWatch *next;
} XmAutoSuggestWatch;

static void outside_press(Widget widget, XtPointer closure, XEvent *event, Boolean *dispatch)
{
    XmAutoSuggestBoxWidget box = closure;
    (void)dispatch;
    if (event->type != ButtonPress || !box->auto_suggest.visible || box->core.being_destroyed) return;
    for (Widget current = widget; current; current = XtParent(current))
        if (current == (Widget)box || current == box->auto_suggest.popup_shell) return;
    /* Observe without consuming: translations and other handlers still receive
     * the original press. No pointer grab or event replay is needed. */
    dismiss(box, event);
}

static void watched_destroyed(Widget widget, XtPointer closure, XtPointer data)
{
    XmAutoSuggestWatch *watch = closure;
    (void)widget; (void)data;
    if (watch->owner) {
        XmAutoSuggestWatch **link = &watch->owner->auto_suggest.outside_watches;
        while (*link && *link != watch) link = &(*link)->next;
        if (*link) *link = watch->next;
    }
    XtFree((char *)watch);
}

static void watch_tree(XmAutoSuggestBoxWidget box, Widget widget)
{
    XmAutoSuggestWatch *watch;
    if (!XtIsWidget(widget) || widget->core.being_destroyed) return;
    for (watch = box->auto_suggest.outside_watches; watch; watch = watch->next)
        if (watch->widget == widget) return;
    watch = (XmAutoSuggestWatch *)XtMalloc(sizeof(*watch));
    if (!watch) return;
    watch->widget = widget; watch->owner = box;
    watch->next = box->auto_suggest.outside_watches;
    box->auto_suggest.outside_watches = watch;
    XtAddCallback(widget, XtNdestroyCallback, watched_destroyed, watch);
    XtInsertEventHandler(widget, ButtonPressMask, False, outside_press, box, XtListHead);
    if (XtIsComposite(widget)) {
        CompositeWidget composite = (CompositeWidget)widget;
        for (Cardinal i = 0; i < composite->composite.num_children; ++i)
            watch_tree(box, composite->composite.children[i]);
    }
    for (Cardinal i = 0; i < widget->core.num_popups; ++i)
        watch_tree(box, widget->core.popup_list[i]);
}

static void outside_created(Widget hooks, XtPointer closure, XtPointer data)
{
    XmAutoSuggestBoxWidget box = closure;
    XtCreateHookData created = data;
    (void)hooks;
    if (box->auto_suggest.watching_outside && created && created->widget)
        watch_tree(box, created->widget);
}

static void start_outside_watch(XmAutoSuggestBoxWidget box)
{
    Widget hooks;
    WidgetList shells = NULL;
    Cardinal count = 0;
    if (box->auto_suggest.watching_outside) return;
    hooks = XtHooksOfDisplay(XtDisplay((Widget)box));
    box->auto_suggest.watching_outside = True;
    XtAddCallback(hooks, XtNcreateHook, outside_created, box);
    XtVaGetValues(hooks, XtNshells, &shells, XtNnumShells, &count, NULL);
    for (Cardinal i = 0; i < count; ++i) watch_tree(box, shells[i]);
}

static void stop_outside_watch(XmAutoSuggestBoxWidget box)
{
    if (!box->auto_suggest.watching_outside) return;
    box->auto_suggest.watching_outside = False;
    XtRemoveCallback(XtHooksOfDisplay(XtDisplay((Widget)box)), XtNcreateHook, outside_created, box);
    while (box->auto_suggest.outside_watches) {
        XmAutoSuggestWatch *watch = box->auto_suggest.outside_watches;
        Widget widget = watch->widget;
        box->auto_suggest.outside_watches = watch->next;
        XtRemoveEventHandler(widget, ButtonPressMask, False, outside_press, box);
        /* A destroy callback list may already be executing. Detach its owner
         * and let the callback release this record rather than freeing its
         * still-snapshotted closure here. */
        if (widget->core.being_destroyed) watch->owner = NULL;
        else {
            XtRemoveCallback(widget, XtNdestroyCallback, watched_destroyed, watch);
            XtFree((char *)watch);
        }
    }
}

static void position_popup(XmAutoSuggestBoxWidget box)
{
    Widget shell = box->auto_suggest.popup_shell;
    Widget list = box->auto_suggest.list;
    Position root_x, root_y;
    Dimension highlight;
    int anchor_width, anchor_top, anchor_bottom;
    XtWidgetGeometry wanted = {0};
    int width, height, screen_width, screen_height;
    long long x_pos, y_pos, max_x, max_y, below, above;
    unsigned rows = box->auto_suggest.item_count;
    if (!shell || !box->auto_suggest.search || !XtIsRealized(box->auto_suggest.search)) return;
    if (rows < 1) rows = 1;
    if (rows > 8) rows = 8;
    XtVaSetValues(list, XmNvisibleItemCount, (int)rows, NULL);
    XtQueryGeometry(list, NULL, &wanted);
    height = (wanted.request_mode & CWHeight) ? wanted.height : list->core.height;
    /* The native scroller owns overflow and keeps long labels within the
     * anchor width; it never expands the popup to a label's natural width. */
    {
        Dimension shadow, margin, spacing;
        Widget horizontal = NULL;
        XtVaGetValues(box->auto_suggest.scroller, XmNshadowThickness, &shadow,
                      XmNscrolledWindowMarginHeight, &margin, XmNspacing, &spacing,
                      XmNhorizontalScrollBar, &horizontal, NULL);
        height += 2 * ((int)shadow + margin);
        if (horizontal && XtIsManaged(horizontal)) height += horizontal->core.height + spacing;
    }
    XtTranslateCoords(box->auto_suggest.search, 0, 0, &root_x, &root_y);
    screen_width = MIN(WidthOfScreen(XtScreen(shell)), SHRT_MAX);
    screen_height = MIN(HeightOfScreen(XtScreen(shell)), SHRT_MAX);
    XtVaGetValues(box->auto_suggest.search, XmNhighlightThickness, &highlight, NULL);
    highlight = MIN(highlight, MIN(box->auto_suggest.search->core.width,
                                   box->auto_suggest.search->core.height) / 2);
    anchor_width = MAX(1, (int)box->auto_suggest.search->core.width - 2 * (int)highlight);
    anchor_top = (int)root_y + highlight;
    anchor_bottom = (int)root_y + (int)box->auto_suggest.search->core.height - highlight;
    width = MIN(anchor_width, screen_width);
    x_pos = (long long)root_x + highlight;
    if (XmDirectionMatch(_XmGetLayoutDirection((Widget)box), XmRIGHT_TO_LEFT))
        x_pos += anchor_width - width;
    max_x = MAX(0, (long long)screen_width - width);
    x_pos = MAX(0LL, MIN(x_pos, max_x));
    below = MAX(0LL, (long long)screen_height - anchor_bottom);
    above = MAX(0LL, MIN((long long)anchor_top, (long long)screen_height));
    if (height > below && above > below) {
        height = MAX(1, (int)MIN((long long)height, above));
        y_pos = (long long)anchor_top - height;
    } else {
        height = MAX(1, (int)MIN((long long)height, below));
        y_pos = anchor_bottom;
    }
    max_y = MAX(0, (long long)screen_height - height);
    y_pos = MAX(0LL, MIN(y_pos, max_y));
    if (x_pos > SHRT_MAX) x_pos = SHRT_MAX;
    if (y_pos > SHRT_MAX) y_pos = SHRT_MAX;
    XtVaSetValues(shell, XtNx, (Position)x_pos, XtNy, (Position)y_pos,
                  XtNwidth, width, XtNheight, height, NULL);
    XtConfigureWidget(box->auto_suggest.scroller, 0, 0,
                      (Dimension)width, (Dimension)height, 0);
}

static void popup_destroyed(Widget widget, XtPointer closure, XtPointer data)
{
    XmAutoSuggestBoxWidget box = closure;
    (void)widget; (void)data;
    box->auto_suggest.popup_shell = NULL;
    box->auto_suggest.list = NULL;
    box->auto_suggest.scroller = NULL;
    box->auto_suggest.visible = False;
    stop_outside_watch(box);
}
static void search_destroyed(Widget widget, XtPointer closure, XtPointer data)
{
    (void)widget; (void)data;
    ((XmAutoSuggestBoxWidget)closure)->auto_suggest.search = NULL;
}

static void create_popup(XmAutoSuggestBoxWidget box)
{
    Widget parent_shell = shell_ancestor((Widget)box);
    if (box->auto_suggest.popup_shell || !parent_shell) return;
    box->auto_suggest.popup_shell = XtVaCreatePopupShell(
        "suggestionsShell", transientShellWidgetClass, parent_shell,
        XtNoverrideRedirect, True, XtNallowShellResize, True,
        XtNsaveUnder, True, XtNborderWidth, 0, NULL);
    if (!box->auto_suggest.popup_shell) return;
    XtAddCallback(box->auto_suggest.popup_shell, XtNdestroyCallback, popup_destroyed, box);
    {
        Arg args[3];
        XtSetArg(args[0], XmNselectionPolicy, XmSINGLE_SELECT);
        XtSetArg(args[1], XmNlistSizePolicy, XmCONSTANT);
        XtSetArg(args[2], XmNvisibleItemCount, 1);
        box->auto_suggest.list = XmCreateScrolledList(box->auto_suggest.popup_shell,
                                                     (char *)"suggestions", args, XtNumber(args));
    }
    if (!box->auto_suggest.list) { XtDestroyWidget(box->auto_suggest.popup_shell); return; }
    box->auto_suggest.scroller = XtParent(box->auto_suggest.list);
    XtVaSetValues(box->auto_suggest.scroller, XmNspacing, 0,
                  XmNscrolledWindowMarginWidth, 0, XmNscrolledWindowMarginHeight, 0, NULL);
    XtAddCallback(box->auto_suggest.list, XmNdefaultActionCallback, list_selected, box);
    XtAddCallback(box->auto_suggest.list, XmNsingleSelectionCallback, list_selected, box);
    XtAddEventHandler(box->auto_suggest.list, KeyPressMask, False, list_events, box);
    XtAddEventHandler(box->auto_suggest.list, FocusChangeMask, False, focus_events, box);
    XtAddEventHandler(box->auto_suggest.popup_shell, FocusChangeMask, False, focus_events, box);
    XtManageChild(box->auto_suggest.list);
}

static void emit_query(XmAutoSuggestBoxWidget box, XEvent *event)
{
    XmAutoSuggestCallbackStruct callback;
    char *query = box->auto_suggest.search != NULL
                      ? XmSearchBoxGetString(box->auto_suggest.search) : NULL;
    callback.reason = XmAUTO_SUGGEST_REASON_QUERY_CHANGED;
    callback.event = event;
    callback.query = query ? query : "";
    callback.generation = box->auto_suggest.generation;
    callback.id = NULL;
    callback.user_data = NULL;
    _XmCallCallbacksUntilDestroyed((Widget)box, XmNautoSuggestQueryChangedCallback, &callback);
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
    box->auto_suggest.laying_out = False;
    box->auto_suggest.committing = False;
    box->auto_suggest.layout_work = 0;
    box->auto_suggest.automatic_width = request->core.width == 0;
    box->auto_suggest.automatic_height = request->core.height == 0;
    box->auto_suggest.no_results_label = XtNewString(no_results_text(box));
    box->auto_suggest.generation = 0;
    box->auto_suggest.visible = False;
    box->auto_suggest.pending_show = False;
    box->auto_suggest.cancel_pending = False;
    box->auto_suggest.cancel_timer = 0;
    box->auto_suggest.focus_timer = 0;
    box->auto_suggest.outside_watches = NULL;
    box->auto_suggest.watching_outside = False;
    box->auto_suggest.no_results = False;
    box->auto_suggest.search = XmVaCreateManagedSearchBox(
        widget, (char *)"search", XmNtraversalOn, True, NULL);
    if (!box->auto_suggest.search) { box->auto_suggest.initializing = False; return; }
    XtAddCallback(box->auto_suggest.search, XtNdestroyCallback, search_destroyed, box);
    XtAddEventHandler(box->auto_suggest.search, FocusChangeMask, False, focus_events, box);
    XtAddCallback(box->auto_suggest.search, XmNsearchQueryChangedCallback,
                  query_changed, box);
    XtAddCallback(box->auto_suggest.search, XmNsearchCommittedCallback,
                  search_committed, box);
    XtInsertEventHandler(box->auto_suggest.search, KeyPressMask, False,
                         search_events, box, XtListHead);
    XtAddEventHandler(box->auto_suggest.search, StructureNotifyMask, False, anchor_events, box);
    for (Widget anchor = widget; anchor; anchor = XtParent(anchor))
        XtAddEventHandler(anchor, StructureNotifyMask, False, anchor_events, box);
    if (box->auto_suggest.automatic_width) box->core.width = box->auto_suggest.search->core.width;
    if (box->auto_suggest.automatic_height) box->core.height = box->auto_suggest.search->core.height;
    box->auto_suggest.initializing = False;
    layout(box);
}

static void Destroy(Widget widget)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)widget;
    stop_outside_watch(box);
    if (box->auto_suggest.layout_work) XtRemoveWorkProc(box->auto_suggest.layout_work);
    if (box->auto_suggest.cancel_timer) XtRemoveTimeOut(box->auto_suggest.cancel_timer);
    if (box->auto_suggest.focus_timer) XtRemoveTimeOut(box->auto_suggest.focus_timer);
    for (Widget anchor = widget; anchor; anchor = XtParent(anchor))
        XtRemoveEventHandler(anchor, StructureNotifyMask, False, anchor_events, box);
    XtFree(box->auto_suggest.no_results_label);
    if (box->auto_suggest.search != NULL) {
        XtRemoveEventHandler(box->auto_suggest.search, FocusChangeMask, False, focus_events, box);
        XtRemoveEventHandler(box->auto_suggest.search, StructureNotifyMask, False, anchor_events, box);
        XtRemoveCallback(box->auto_suggest.search,
                         XmNsearchQueryChangedCallback, query_changed, box);
        XtRemoveCallback(box->auto_suggest.search,
                         XmNsearchCommittedCallback, search_committed, box);
        XtRemoveCallback(box->auto_suggest.search, XtNdestroyCallback, search_destroyed, box);
        XtRemoveEventHandler(box->auto_suggest.search, KeyPressMask, False,
                             search_events, box);
        box->auto_suggest.search = NULL;
    }
    free_items(box);
    if (box->auto_suggest.popup_shell) {
        Widget popup = box->auto_suggest.popup_shell;
        XtRemoveCallback(popup, XtNdestroyCallback, popup_destroyed, box);
        if (!popup->core.being_destroyed && (!shell_ancestor(widget) || !shell_ancestor(widget)->core.being_destroyed))
            XtDestroyWidget(popup);
    }
}

static void layout(XmAutoSuggestBoxWidget box)
{
    if (box->auto_suggest.initializing || box->auto_suggest.laying_out)
        return;
    if (!box->auto_suggest.search) return;
    box->auto_suggest.laying_out = True;
    XmeConfigureObject(box->auto_suggest.search, 0, 0, box->core.width ? box->core.width : 1,
                       box->core.height ? box->core.height : 1, 0);
    box->auto_suggest.laying_out = False;
    if (box->auto_suggest.visible) position_popup(box);
}

static void preferred(XmAutoSuggestBoxWidget box, Dimension *width, Dimension *height)
{
    XtWidgetGeometry wanted = {0};
    if (!box->auto_suggest.search) { *width = *height = 1; return; }
    XtQueryGeometry(box->auto_suggest.search, NULL, &wanted);
    *width = MAX(1, (wanted.request_mode & CWWidth) ? wanted.width : box->auto_suggest.search->core.width);
    *height = MAX(1, (wanted.request_mode & CWHeight) ? wanted.height : box->auto_suggest.search->core.height);
}
static Boolean layout_work(XtPointer closure)
{
    XmAutoSuggestBoxWidget box = closure;
    Dimension width, height;
    XtWidgetGeometry request = {0}, reply = {0};
    box->auto_suggest.layout_work = 0;
    preferred(box, &width, &height);
    request.request_mode = CWWidth | CWHeight;
    request.width = box->auto_suggest.automatic_width ? width : box->core.width;
    request.height = box->auto_suggest.automatic_height ? height : box->core.height;
    if (request.width != box->core.width || request.height != box->core.height) {
        if (XtMakeGeometryRequest((Widget)box, &request, &reply) == XtGeometryAlmost) {
            if (reply.request_mode & CWWidth) request.width = reply.width;
            if (reply.request_mode & CWHeight) request.height = reply.height;
            XtMakeGeometryRequest((Widget)box, &request, NULL);
        }
    }
    layout(box);
    return True;
}
static void schedule_layout(XmAutoSuggestBoxWidget box)
{
    if (!box->auto_suggest.initializing && !box->core.being_destroyed && !box->auto_suggest.layout_work)
        box->auto_suggest.layout_work = XtAppAddWorkProc(XtWidgetToApplicationContext((Widget)box), layout_work, box);
}
static void Resize(Widget widget) { layout((XmAutoSuggestBoxWidget)widget); }
static void ChangeManaged(Widget widget) { schedule_layout((XmAutoSuggestBoxWidget)widget); }
static void anchor_events(Widget widget, XtPointer closure, XEvent *event, Boolean *dispatch)
{
    XmAutoSuggestBoxWidget box = closure;
    (void)widget; (void)dispatch;
    if (event->type == MapNotify && box->auto_suggest.pending_show) {
        XmAutoSuggestBoxShow((Widget)box); return;
    }
    if (!box->auto_suggest.visible) return;
    if (event->type == UnmapNotify) { dismiss(box, event); return; }
    if (event->type == ConfigureNotify) position_popup(box);
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count)
{
    XmAutoSuggestBoxWidget old = (XmAutoSuggestBoxWidget)old_widget;
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)widget;
    Dimension width, height;
    (void)request;
    if (!XtIsSensitive(widget)) {
        /* Latch cancellation even if an ancestor is re-enabled before the
         * event loop runs. Notify only after the SetValues stack unwinds. */
        box->auto_suggest.cancel_pending = True;
        box->auto_suggest.pending_show = False;
        if (box->auto_suggest.list) XtSetSensitive(box->auto_suggest.list, False);
        if (!box->auto_suggest.cancel_timer)
            box->auto_suggest.cancel_timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget),
                                                             0, cancel_popup, box);
    }
    for (Cardinal i = 0; i < *count; ++i) {
        if (!strcmp(args[i].name, XmNwidth)) box->auto_suggest.automatic_width = args[i].value == 0;
        if (!strcmp(args[i].name, XmNheight)) box->auto_suggest.automatic_height = args[i].value == 0;
    }
    if (old->auto_suggest.no_results_label != box->auto_suggest.no_results_label) {
        String replacement = XtNewString(no_results_text(box));
        XmString label = NULL;

        if (replacement == NULL) {
            box->auto_suggest.no_results_label = old->auto_suggest.no_results_label;
        } else if (box->auto_suggest.no_results && box->auto_suggest.list) {
            label = XmStringCreateLocalized(replacement);
            if (label == NULL) {
                XtFree(replacement);
                box->auto_suggest.no_results_label = old->auto_suggest.no_results_label;
            }
        }
        if (box->auto_suggest.no_results_label != old->auto_suggest.no_results_label) {
            XtFree(old->auto_suggest.no_results_label);
            box->auto_suggest.no_results_label = replacement;
        }
        if (box->auto_suggest.no_results && box->auto_suggest.list) {
            if (label != NULL) {
                XmListReplaceItemsPos(box->auto_suggest.list, &label, 1, 1);
                XmStringFree(label);
            }
        }
    }
    preferred(box, &width, &height);
    if (!box->core.width) box->core.width = width;
    if (!box->core.height) box->core.height = height;
    schedule_layout(box);
    return False;
}
static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request,
                                        XtWidgetGeometry *reply)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)XtParent(child);
    (void)reply;
    if (!(request->request_mode & XtCWQueryOnly)) schedule_layout(box);
    return XtGeometryNo;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)widget;
    preferred(box, &reply->width, &reply->height);
    if (!box->auto_suggest.automatic_width) reply->width = box->core.width;
    if (!box->auto_suggest.automatic_height) reply->height = box->core.height;
    return XmeReplyToQueryGeometry(widget, request, reply);
}

static void query_changed(Widget widget, XtPointer client, XtPointer call)
{
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)client;
    XmSearchBoxCallbackStruct *search_callback =
        (XmSearchBoxCallbackStruct *)call;
    (void)widget;
    if (!box || !XmIsAutoSuggestBox((Widget)box) || box->core.being_destroyed) return;
    unsigned long generation = ++box->auto_suggest.generation;
    if (box->auto_suggest.committing) return;
    if (!dismiss(box, search_callback ? search_callback->event : NULL)) return;
    if (generation != box->auto_suggest.generation) return;
    free_items(box);
    box->auto_suggest.no_results = False;
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
    if (!box || !XmIsAutoSuggestBox((Widget)box) || box->core.being_destroyed) return;
    (void)call;
    if (box->auto_suggest.visible && box->auto_suggest.list &&
        !box->auto_suggest.no_results &&
        XmListGetSelectedPos(box->auto_suggest.list, &positions, &count) &&
        count == 1 && positions && positions[0] > 0)
        XmAutoSuggestBoxCommit((Widget)box, (Cardinal)(positions[0] - 1),
                               search_callback ? search_callback->event : NULL);
    XtFree((char *)positions);
}

static void list_selected(Widget widget, XtPointer client, XtPointer call)
{
    XmListCallbackStruct *selection = (XmListCallbackStruct *)call;
    XmAutoSuggestBoxWidget box = (XmAutoSuggestBoxWidget)client;
    (void)widget;
    if (!box || !XmIsAutoSuggestBox((Widget)box) || box->core.being_destroyed) return;
    if (box->auto_suggest.visible && selection && selection->item_position > 0)
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
    if (!box || !XmIsAutoSuggestBox((Widget)box) || box->core.being_destroyed) return;
    if (box == NULL || event == NULL || dispatch == NULL ||
        !XmIsAutoSuggestBox((Widget)box))
	return;
    if (event->type != KeyPress)
        return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_Escape && box->auto_suggest.visible) {
        if (dispatch) *dispatch = False;
        dismiss(box, event);
    } else if (key == XK_Down && box->auto_suggest.visible && !box->auto_suggest.no_results) {
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
    if (!box || !XmIsAutoSuggestBox((Widget)box) || box->core.being_destroyed) return;
    if (box == NULL || event == NULL || !XmIsAutoSuggestBox((Widget)box))
	return;
    if (event->type != KeyPress)
        return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_Escape) {
        if (dispatch) *dispatch = False;
        if (dismiss(box, event)) XmProcessTraversal(box->auto_suggest.search, XmTRAVERSE_CURRENT);
    } else if (key == XK_Tab || key == XK_ISO_Left_Tab) {
        XmTraversalDirection direction = (key == XK_ISO_Left_Tab ||
            (event->xkey.state & ShiftMask)) ? XmTRAVERSE_PREV_TAB_GROUP :
                                             XmTRAVERSE_NEXT_TAB_GROUP;
        if (dispatch) *dispatch = False;
        /* Traverse relative to the field in its owning shell, not within the
         * separate popup shell. Dismissal callbacks may destroy the owner. */
        if (dismiss(box, event))
            XmProcessTraversal(box->auto_suggest.search, direction);
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
    XmAutoSuggestStoredItem *replacement = NULL;
    XmString *labels = NULL;
    XmString no_results = NULL;
    Cardinal copied = 0;
    if (!widget || !XmIsAutoSuggestBox(widget) || (count && !items) || widget->core.being_destroyed)
        return False;
    box = (XmAutoSuggestBoxWidget)widget;
    if (generation != box->auto_suggest.generation || count > INT_MAX ||
        (size_t)count > SIZE_MAX / sizeof(*replacement) ||
        (size_t)count > (size_t)UINT_MAX / sizeof(*replacement)) return False;
    if (count) {
        replacement = (XmAutoSuggestStoredItem *)XtCalloc(count, sizeof(*replacement));
        labels = (XmString *)XtCalloc(count, sizeof(*labels));
        if (!replacement || !labels) goto failed;
        for (Cardinal i = 0; i < count; ++i) {
            replacement[i].id = XtNewString(items[i].id ? items[i].id : "");
            replacement[i].label = items[i].label ? XmStringCopy(items[i].label) : XmStringCreateLocalized("");
            replacement[i].user_data = items[i].user_data;
            copied = i + 1;
            if (!replacement[i].id || !replacement[i].label) goto failed;
            labels[i] = replacement[i].label;
        }
    }
    if (!count) {
        no_results = XmStringCreateLocalized((char *)no_results_text(box));
        if (!no_results) goto failed;
    }
    create_popup(box);
    if (!box->auto_suggest.list) goto failed;
    free_items(box);
    box->auto_suggest.items = replacement;
    box->auto_suggest.item_count = count;
    box->auto_suggest.no_results = count == 0;
    XmListDeleteAllItems(box->auto_suggest.list);
    if (count) XmListAddItems(box->auto_suggest.list, labels, (int)count, 1);
    else {
        XmListAddItem(box->auto_suggest.list, no_results, 1);
        XmStringFree(no_results);
    }
    XtFree((char *)labels);
    XmListDeselectAllItems(box->auto_suggest.list);
    XtSetSensitive(box->auto_suggest.list, count != 0 && XtIsSensitive(widget) &&
                   !box->auto_suggest.cancel_pending);
    XmAutoSuggestBoxShow(widget);
    return True;
failed:
    if (no_results) XmStringFree(no_results);
    for (Cardinal i = 0; i < copied; ++i) {
        XtFree(replacement[i].id); XmStringFree(replacement[i].label);
    }
    XtFree((char *)replacement); XtFree((char *)labels);
    return False;
}

Cardinal XmAutoSuggestBoxGetSuggestionCount(Widget widget)
{
    return widget && XmIsAutoSuggestBox(widget) && !widget->core.being_destroyed
               ? ((XmAutoSuggestBoxWidget)widget)->auto_suggest.item_count : 0;
}

unsigned long XmAutoSuggestBoxGetGeneration(Widget widget)
{
    return widget && XmIsAutoSuggestBox(widget) && !widget->core.being_destroyed
               ? ((XmAutoSuggestBoxWidget)widget)->auto_suggest.generation : 0;
}

void XmAutoSuggestBoxShow(Widget widget)
{
    XmAutoSuggestBoxWidget box;
    if (!widget || !XmIsAutoSuggestBox(widget))
        return;
    box = (XmAutoSuggestBoxWidget)widget;
    if (!XtIsSensitive(widget) || box->auto_suggest.cancel_pending || widget->core.being_destroyed ||
        (!box->auto_suggest.item_count && !box->auto_suggest.no_results)) return;
    box->auto_suggest.pending_show = True;
    if (!box->auto_suggest.search || !XtIsRealized(box->auto_suggest.search)) return;
    {
        XWindowAttributes attributes;
        if (!XGetWindowAttributes(XtDisplay(widget), XtWindow(box->auto_suggest.search), &attributes) ||
            attributes.map_state != IsViewable) return;
    }
    box->auto_suggest.pending_show = False;
    create_popup(box);
    if (!box->auto_suggest.popup_shell)
        return;
    XtSetSensitive(box->auto_suggest.list, box->auto_suggest.item_count != 0);
    position_popup(box);
    XtPopup(box->auto_suggest.popup_shell, XtGrabNone);
    box->auto_suggest.visible = True;
    start_outside_watch(box);
}

static void cancel_popup(XtPointer closure, XtIntervalId *id)
{
    XmAutoSuggestBoxWidget box = closure;
    (void)id;
    if (box == NULL || box->core.being_destroyed)
        return;
    box->auto_suggest.cancel_timer = 0;
    box->auto_suggest.cancel_pending = False;
    dismiss(box, NULL);
}

static Boolean dismiss(XmAutoSuggestBoxWidget box, XEvent *event)
{
    Widget widget = (Widget)box;
    Boolean alive;
    XmAutoSuggestCallbackStruct callback;
    char *query;
    box->auto_suggest.pending_show = False;
    if (!box->auto_suggest.visible) return True;
    box->auto_suggest.visible = False;
    stop_outside_watch(box);
    if (box->auto_suggest.popup_shell)
        XtPopdown(box->auto_suggest.popup_shell);
    query = box->auto_suggest.search != NULL
                ? XmSearchBoxGetString(box->auto_suggest.search) : NULL;
    callback.reason = XmAUTO_SUGGEST_REASON_DISMISSED;
    callback.event = event;
    callback.query = query ? query : "";
    callback.generation = box->auto_suggest.generation;
    callback.id = NULL;
    callback.user_data = NULL;
    alive = _XmCallCallbacksUntilDestroyed(widget, XmNautoSuggestDismissedCallback, &callback);
    XtFree(query);
    return alive;
}

void XmAutoSuggestBoxDismiss(Widget widget, XEvent *event)
{
    if (widget && XmIsAutoSuggestBox(widget) && !widget->core.being_destroyed)
        dismiss((XmAutoSuggestBoxWidget)widget, event);
}
static void commit_destroyed(Widget widget, XtPointer closure, XtPointer data)
{
    (void)widget; (void)data; *(Boolean *)closure = True;
}
void XmAutoSuggestBoxCommit(Widget widget, Cardinal index, XEvent *event)
{
    XmAutoSuggestBoxWidget box;
    XmAutoSuggestCallbackStruct callback;
    char *query, *label, *id;
    XtPointer user_data;
    Boolean destroyed = False, alive;
    if (!widget || !XmIsAutoSuggestBox(widget) || !XtIsSensitive(widget) || widget->core.being_destroyed) return;
    box = (XmAutoSuggestBoxWidget)widget;
    if (box->auto_suggest.committing || box->auto_suggest.cancel_pending ||
        index >= box->auto_suggest.item_count) return;
    id = XtNewString(box->auto_suggest.items[index].id);
    user_data = box->auto_suggest.items[index].user_data;
    label = text_copy(box->auto_suggest.items[index].label);
    box->auto_suggest.committing = True;
    XtAddCallback(widget, XmNdestroyCallback, commit_destroyed, &destroyed);
    XmSearchBoxSetString(box->auto_suggest.search, label);
    if (destroyed) { XtFree(id); XtFree(label); return; }
    XtRemoveCallback(widget, XmNdestroyCallback, commit_destroyed, &destroyed);
    if (widget->core.being_destroyed) { XtFree(id); XtFree(label); return; }
    box->auto_suggest.committing = False;
    query = XmSearchBoxGetString(box->auto_suggest.search);
    if (!query || strcmp(query, label)) { XtFree(id); XtFree(label); XtFree(query); return; }
    XtFree(label);
    callback.reason = XmAUTO_SUGGEST_REASON_COMMITTED;
    callback.event = event;
    callback.query = query;
    callback.generation = box->auto_suggest.generation;
    callback.id = id;
    callback.user_data = user_data;
    alive = _XmCallCallbacksUntilDestroyed(widget, XmNautoSuggestCommittedCallback, &callback);
    XtFree(id); XtFree(query);
    if (!alive || callback.generation != box->auto_suggest.generation) return;
    if (!dismiss(box, event)) return;
    XmProcessTraversal(box->auto_suggest.search, XmTRAVERSE_CURRENT);
}

Widget XmAutoSuggestBoxGetSearchField(Widget widget)
{
    return widget && XmIsAutoSuggestBox(widget) && !widget->core.being_destroyed
               ? ((XmAutoSuggestBoxWidget)widget)->auto_suggest.search : NULL;
}
