#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/SearchBoxP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <string.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);
static void query_changed(Widget, XtPointer, XtPointer);
static void committed(Widget, XtPointer, XtPointer);
#define Offset(field) XtOffsetOf(XmSearchBoxRec, field)
static XtResource resources[] = {
    {XmNsearchPlaceholder, XmCSearchPlaceholder, XmRXmString, sizeof(XmString), Offset(search.placeholder), XmRImmediate, NULL},
    {XmNsearchClearButton, XmCSearchClearButton, XmRBoolean, sizeof(Boolean), Offset(search.clear_button), XmRImmediate, (XtPointer)True},
    {XmNsearchQueryChangedCallback, XmCSearchQueryChangedCallback, XmRCallback, sizeof(XtCallbackList), Offset(search.query_changed_callback), XmRPointer, NULL},
    {XmNsearchCommittedCallback, XmCSearchCommittedCallback, XmRCallback, sizeof(XtCallbackList), Offset(search.committed_callback), XmRPointer, NULL},
    {XmNsearchClearedCallback, XmCSearchClearedCallback, XmRCallback, sizeof(XtCallbackList), Offset(search.cleared_callback), XmRPointer, NULL}
};
externaldef(xmsearchboxclassrec) XmSearchBoxClassRec xmSearchBoxClassRec = {
    {(WidgetClass)&xmTextFieldClassRec, "XmSearchBox", sizeof(XmSearchBoxRec), NULL,
     NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0, resources,
     XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True, False,
     Destroy, NULL, Redisplay, SetValues, NULL, XtInheritSetValuesAlmost, NULL,
     NULL, XtVersion, NULL, XtInheritTranslations, NULL, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight, XtInheritTranslations,
     NULL, NULL, 0, NULL}, {NULL}, {NULL}};
externaldef(xmsearchboxwidgetclass) WidgetClass xmSearchBoxWidgetClass =
    (WidgetClass)&xmSearchBoxClassRec;

static XmString copy_label(XmString source) {
    char *text; XmString result;
    if (!source) return XmStringCreateLocalized("");
    text = XmStringUnparse(source, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                           NULL, 0, XmOUTPUT_ALL);
    if (!text) return XmStringCreateLocalized("");
    result = XmStringCreateLocalized(text); XtFree(text); return result;
}
static char *query_copy(Widget widget) {
    char *value = XmTextFieldGetString(widget);
    return value ? value : XtNewString("");
}
static void emit(Widget widget, XtCallbackList callbacks, int reason, XEvent *event) {
    XmSearchBoxCallbackStruct callback; char *query = query_copy(widget);
    callback.reason = reason; callback.event = event; callback.query = query;
    XtCallCallbackList(widget, callbacks, &callback);
    XtFree(query);
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget;
    (void)request; (void)args; (void)count;
    box->search.placeholder = copy_label(box->search.placeholder);
    XtAddCallback(widget, XmNvalueChangedCallback, query_changed, NULL);
    XtAddCallback(widget, XmNactivateCallback, committed, NULL);
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events, NULL);
}
static void Destroy(Widget widget) {
    XtRemoveCallback(widget, XmNvalueChangedCallback, query_changed, NULL);
    XtRemoveCallback(widget, XmNactivateCallback, committed, NULL);
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events, NULL);
    /* Xt releases XmNsearchPlaceholder resource storage after Destroy. */
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget;
    XtExposeProc expose = ((WidgetClass)&xmTextFieldClassRec)->core_class.expose;
    if (expose) expose(widget, event, region);
    if (box->search.clear_button && XtWindow(widget)) {
        char *value = XmTextFieldGetString(widget);
        if (value && *value) {
            Display *display = XtDisplay(widget); Window window = XtWindow(widget);
            int cx = (int)box->core.width - 13;
            XDrawLine(display, window, box->text.gc, cx - 4, 8, cx + 4, 16);
            XDrawLine(display, window, box->text.gc, cx + 4, 8, cx - 4, 16);
        }
        if (value) XtFree(value);
    }
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmSearchBoxWidget old = (XmSearchBoxWidget)old_widget, box = (XmSearchBoxWidget)widget;
    (void)request; (void)args; (void)count;
    if (old->search.placeholder != box->search.placeholder)
        box->search.placeholder = copy_label(box->search.placeholder);
    return old->search.placeholder != box->search.placeholder ||
           old->search.clear_button != box->search.clear_button;
}
static void query_changed(Widget widget, XtPointer closure, XtPointer data) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget;
    (void)closure; (void)data;
    if (!box->search.clearing) emit(widget, box->search.query_changed_callback,
                                    XmSEARCH_REASON_QUERY_CHANGED, NULL);
}
static void committed(Widget widget, XtPointer closure, XtPointer data) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget;
    (void)closure; (void)data;
    emit(widget, box->search.committed_callback, XmSEARCH_REASON_COMMITTED, NULL);
}
static void Events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) {
    XmSearchBoxWidget box = (XmSearchBoxWidget)widget; KeySym key; char input[8];
    (void)data;
    if (!XtIsSensitive(widget)) return;
    if (event->type == ButtonPress && box->search.clear_button &&
        event->xbutton.x >= (int)box->core.width - 28) {
        XmSearchBoxClear(widget); if (dispatch) *dispatch = False; return;
    }
    if (event->type != KeyPress) return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_Escape && box->search.clear_button) {
        XmSearchBoxClear(widget); if (dispatch) *dispatch = False;
    }
}
char *XmSearchBoxGetString(Widget widget) { return XmIsSearchBox(widget) ? XmTextFieldGetString(widget) : NULL; }
void XmSearchBoxSetString(Widget widget, const char *value) { if (XmIsSearchBox(widget)) XmTextFieldSetString(widget, (char *)(value ? value : "")); }
void XmSearchBoxClear(Widget widget) {
    XmSearchBoxWidget box;
    if (!XmIsSearchBox(widget) || !XtIsSensitive(widget)) return;
    box = (XmSearchBoxWidget)widget; box->search.clearing = True;
    XmTextFieldSetString(widget, (char *)""); box->search.clearing = False;
    emit(widget, box->search.cleared_callback, XmSEARCH_REASON_CLEARED, NULL);
    emit(widget, box->search.query_changed_callback, XmSEARCH_REASON_QUERY_CHANGED, NULL);
}
void XmSearchBoxCommit(Widget widget, XEvent *event) {
    XmSearchBoxWidget box;
    if (!XmIsSearchBox(widget) || !XtIsSensitive(widget)) return;
    box = (XmSearchBoxWidget)widget;
    emit(widget, box->search.committed_callback, XmSEARCH_REASON_COMMITTED, event);
}
Widget XmCreateSearchBox(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmSearchBoxWidgetClass, parent, args, count); }
Widget XmVaCreateSearchBox(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmSearchBoxWidgetClass,parent,False,v,n); va_end(v); return w; }
Widget XmVaCreateManagedSearchBox(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmSearchBoxWidgetClass,parent,True,v,n); va_end(v); return w; }
