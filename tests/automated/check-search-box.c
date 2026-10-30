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
    XmSearchBoxSetString(search, "clear me");
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.action_count == 2 && info.text_length == 8);
    action_name = XmAccessibilityActionName(accessibility, id, 1);
    CHECK(action_name && strcmp(action_name, "clear") == 0); XtFree(action_name);
    CHECK(XmAccessibilityAction(accessibility, id, 1));
    CHECK(cleared == 2);
    XmSearchBoxSetString(search, "readonly");
    XtVaSetValues(search, XmNeditable, False, NULL);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.action_count == 1);
    CHECK(!XmAccessibilityAction(accessibility, id, 1));
    XmAccessibleInfoClear(&info); XmAccessibilityDestroy(accessibility);
    XtSetSensitive(search, False); XmSearchBoxCommit(search, NULL); CHECK(committed == 2);
    XtDestroyWidget(shell); XtCloseDisplay(XtDisplay(shell)); XtDestroyApplicationContext(app);
    printf("PASS search box: %u checks\n", checks); return EXIT_SUCCESS;
}
