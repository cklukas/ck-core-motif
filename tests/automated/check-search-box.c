#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/Accessible.h>
#include <Xm/SearchBox.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks, changed, committed, cleared, callback_errors;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } ++checks; } while (0)
static void on_changed(Widget w, XtPointer c, XtPointer data) { XmSearchBoxCallbackStruct *cb = data; (void)w; (void)c; if (!cb || cb->reason != XmSEARCH_REASON_QUERY_CHANGED) ++callback_errors; ++changed; }
static void on_committed(Widget w, XtPointer c, XtPointer data) { XmSearchBoxCallbackStruct *cb = data; (void)w; (void)c; if (!cb || cb->reason != XmSEARCH_REASON_COMMITTED) ++callback_errors; ++committed; }
static void on_cleared(Widget w, XtPointer c, XtPointer data) { XmSearchBoxCallbackStruct *cb = data; (void)w; (void)c; if (!cb || cb->reason != XmSEARCH_REASON_CLEARED) ++callback_errors; ++cleared; }
static unsigned later, destroyed_count;
static void destroy_search(Widget w, XtPointer c, XtPointer data) {
    (void)c; (void)data; ++destroyed_count; XtDestroyWidget(w);
}
static void unexpected_callback(Widget w, XtPointer c, XtPointer data) {
    (void)w; (void)c; (void)data; ++later;
}
static void reject_clear(Widget w, XtPointer c, XtPointer data) {
    XmTextVerifyCallbackStruct *verify = data;
    (void)w; (void)c; verify->doit = False;
}
static unsigned snapshots;
static void replace_query(Widget w, XtPointer c, XtPointer data) {
    XmSearchBoxCallbackStruct *cb = data;
    (void)c;
    if (!cb || strcmp(cb->query, "before") != 0) ++callback_errors;
    XmSearchBoxSetString(w, "after");
    XtRemoveAllCallbacks(w, XmNsearchCommittedCallback);
}
static void inspect_snapshot(Widget w, XtPointer c, XtPointer data) {
    XmSearchBoxCallbackStruct *cb = data;
    (void)w; (void)c;
    if (!cb || strcmp(cb->query, "before") != 0) ++callback_errors;
    ++snapshots;
}
static int lifetime_checks(Widget parent) {
    const char *resources[] = {XmNsearchCommittedCallback,
        XmNsearchQueryChangedCallback, XmNsearchClearedCallback,
        XmNvalueChangedCallback};
    unsigned i;
    Widget search;
    char *value;
    for (i = 0; i < XtNumber(resources); ++i) {
        search = XmVaCreateManagedSearchBox(parent, "lifetime", NULL);
        XmSearchBoxSetString(search, "before");
        later = destroyed_count = 0;
        XtAddCallback(search, resources[i], destroy_search, NULL);
        XtAddCallback(search, resources[i], unexpected_callback, NULL);
        if (i == 0) XmSearchBoxCommit(search, NULL);
        else if (i == 1) XmSearchBoxSetString(search, "changed");
        else {
            XtAddCallback(search, XmNsearchQueryChangedCallback,
                          unexpected_callback, NULL);
            XmSearchBoxClear(search);
        }
        CHECK(destroyed_count == 1 && later == 0);
    }
    search = XmVaCreateManagedSearchBox(parent, "snapshot", NULL);
    XmSearchBoxSetString(search, "before");
    XtAddCallback(search, XmNsearchCommittedCallback, replace_query, NULL);
    XtAddCallback(search, XmNsearchCommittedCallback, inspect_snapshot, NULL);
    XmSearchBoxCommit(search, NULL);
    CHECK(snapshots == 1 && callback_errors == 0);
    XmSearchBoxCommit(search, NULL);
    CHECK(snapshots == 1);
    value = XmSearchBoxGetString(search);
    CHECK(value && strcmp(value, "after") == 0); XtFree(value);
    XtAddCallback(search, XmNmodifyVerifyCallback, reject_clear, NULL);
    XtAddCallback(search, XmNsearchClearedCallback, unexpected_callback, NULL);
    XtAddCallback(search, XmNsearchQueryChangedCallback, unexpected_callback, NULL);
    later = 0;
    XmSearchBoxClear(search);
    value = XmSearchBoxGetString(search);
    CHECK(value && strcmp(value, "after") == 0 && later == 0); XtFree(value);
    XtDestroyWidget(search);
    search = XmVaCreateManagedSearchBox(parent, "escapeLifetime", NULL);
    XmSearchBoxSetString(search, "before");
    later = destroyed_count = 0;
    XtAddCallback(search, XmNsearchClearedCallback, destroy_search, NULL);
    XtAddCallback(search, XmNsearchClearedCallback, unexpected_callback, NULL);
    XtAddCallback(search, XmNsearchQueryChangedCallback, unexpected_callback, NULL);
    {
        XEvent event;
        memset(&event, 0, sizeof(event));
        event.xkey.type = KeyPress;
        event.xkey.display = XtDisplay(search);
        event.xkey.window = XtWindow(search);
        event.xkey.root = RootWindowOfScreen(XtScreen(search));
        event.xkey.same_screen = True;
        event.xkey.keycode = XKeysymToKeycode(XtDisplay(search), XK_Escape);
        XtDispatchEvent(&event);
    }
    CHECK(destroyed_count == 1 && later == 0);
    return EXIT_SUCCESS;
}
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, row, search; XmString placeholder;
    XmAccessibility *accessibility; XmAccessibleInfo info; XmAccessibleId id; char *value, *action_name;
    (void)argv;
    shell = XtVaAppInitialize(&app, "SearchBoxTest", NULL, 0, &argc, argv, NULL,
                              XmNwidth, 300, XmNheight, 80, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0); XtManageChild(row);
    placeholder = XmStringCreateLocalized((char *)"Search");
    search = XmVaCreateManagedSearchBox(row, (char *)"search",
                                        XmNsearchPlaceholder, placeholder, NULL);
    {
        XmString stored, replacement = XmStringCreateLtoR("Find\nitems", "search-tag");
        XtVaSetValues(search, XmNsearchPlaceholder, replacement, NULL);
        XtVaGetValues(search, XmNsearchPlaceholder, &stored, NULL);
        CHECK(XmStringCompare(replacement, stored));
        XmStringFree(replacement);
    }
    XmStringFree(placeholder);
    XtAddCallback(search, XmNsearchQueryChangedCallback, on_changed, NULL);
    XtAddCallback(search, XmNsearchCommittedCallback, on_committed, NULL);
    XtAddCallback(search, XmNsearchClearedCallback, on_cleared, NULL);
    XtRealizeWidget(shell); XSync(XtDisplay(shell), False);
    value = XmSearchBoxGetString(search); CHECK(value && value[0] == '\0'); XtFree(value);
    XmSearchBoxSetString(search, "alpha"); CHECK(changed >= 1);
    XmSearchBoxCommit(search, NULL); CHECK(committed == 1);
    XmSearchBoxClear(search); CHECK(cleared == 1 && changed >= 2);
    CHECK(callback_errors == 0);
    accessibility = XmAccessibilityCreate(shell, "SearchBoxTest"); CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, search);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_TEXT && info.action_count == 1 && info.text_length == 0);
    action_name = XmAccessibilityActionName(accessibility, id, 0);
    CHECK(action_name && strcmp(action_name, "commit") == 0); XtFree(action_name);
    XmAccessibleInfoClear(&info); CHECK(XmAccessibilityAction(accessibility, id, 0));
    CHECK(committed == 2);
    value = XmAccessibilityActionName(accessibility,id,0);
    CHECK(value && strcmp(value,"commit")==0);XtFree(value);
    XmSearchBoxSetString(search,"semantic clear");
    CHECK(XmAccessibilityQuery(accessibility,id,&info));
    CHECK(info.action_count==2);XmAccessibleInfoClear(&info);
    value=XmAccessibilityActionName(accessibility,id,1);
    CHECK(value && strcmp(value,"clear")==0);XtFree(value);
    CHECK(!XmAccessibilityAction(accessibility,id,2));
    XtVaSetValues(search,XmNeditable,False,NULL);
    CHECK(XmAccessibilityQuery(accessibility,id,&info));
    CHECK(info.action_count==1);XmAccessibleInfoClear(&info);
    CHECK(!XmAccessibilityAction(accessibility,id,1));
    XtVaSetValues(search,XmNeditable,True,XmNsearchClearButton,False,NULL);
    CHECK(!XmAccessibilityAction(accessibility,id,1));
    XtVaSetValues(search,XmNsearchClearButton,True,NULL);
    XtSetSensitive(row,False);
    CHECK(!XmAccessibilityAction(accessibility,id,1));
    XtSetSensitive(row,True);
    CHECK(XmAccessibilityAction(accessibility,id,1));
    value=XmSearchBoxGetString(search);CHECK(value && !*value);XtFree(value);
    CHECK(cleared==2);
    CHECK(!XmAccessibilityAction(accessibility,id,1));
    XmAccessibilityDestroy(accessibility);
    XtSetSensitive(search, False); XmSearchBoxCommit(search, NULL); CHECK(committed == 2);
    CHECK(lifetime_checks(row) == EXIT_SUCCESS);
    { Display *display = XtDisplay(shell);
      XtDestroyWidget(shell); XtCloseDisplay(display); }
    XtDestroyApplicationContext(app);
    printf("PASS search box: %u checks\n", checks); return EXIT_SUCCESS;
}
