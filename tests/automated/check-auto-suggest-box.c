#include <X11/IntrinsicP.h>
#include <Xm/Accessible.h>
#include <Xm/AutoSuggestBox.h>
#include <Xm/RowColumn.h>
#include <Xm/SearchBox.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks;
static unsigned queries;
static unsigned commits;
static unsigned dismissals;
static char last_id[32];
static Widget box;
static Boolean callback_failed;

#define CHECK(x) do { \
    if (!(x)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); \
        return EXIT_FAILURE; \
    } \
    ++checks; \
} while (0)

static void query_changed(Widget widget, XtPointer client, XtPointer call)
{
    XmAutoSuggestCallbackStruct *callback = (XmAutoSuggestCallbackStruct *)call;
    XmString apple, apricot;
    XmAutoSuggestItem items[2];
    (void)widget;
    (void)client;
    ++queries;
    if (!callback || !callback->query || strcmp(callback->query, "zz") == 0)
        return;
    apple = XmStringCreateLocalized((char *)"Apple");
    apricot = XmStringCreateLocalized((char *)"Apricot");
    items[0] = (XmAutoSuggestItem){"apple", apple, NULL};
    items[1] = (XmAutoSuggestItem){"apricot", apricot, NULL};
    if (!XmAutoSuggestBoxSetSuggestions(box, items, 2, callback->generation))
        callback_failed = True;
    XmStringFree(apple);
    XmStringFree(apricot);
}

static void committed(Widget widget, XtPointer client, XtPointer call)
{
    XmAutoSuggestCallbackStruct *callback = (XmAutoSuggestCallbackStruct *)call;
    (void)widget;
    (void)client;
    ++commits;
    if (callback && callback->id)
        snprintf(last_id, sizeof(last_id), "%s", callback->id);
}

static void dismissed(Widget widget, XtPointer client, XtPointer call)
{
    (void)widget;
    (void)client;
    (void)call;
    ++dismissals;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row, search;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;
    unsigned long generation;
    char *value;

    (void)argv;
    shell = XtVaAppInitialize(&app, "AutoSuggestBoxTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 360, XmNheight, 160, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    box = XmVaCreateManagedAutoSuggestBox(row, (char *)"suggestions",
                                          XmNwidth, 260, XmNheight, 32, NULL);
    XtAddCallback(box, XmNautoSuggestQueryChangedCallback, query_changed, NULL);
    XtAddCallback(box, XmNautoSuggestCommittedCallback, committed, NULL);
    XtAddCallback(box, XmNautoSuggestDismissedCallback, dismissed, NULL);
    search = XmAutoSuggestBoxGetSearchField(box);
    CHECK(search != NULL && XmIsSearchBox(search));

    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);
    XmSearchBoxSetString(search, "ap");
    generation = XmAutoSuggestBoxGetGeneration(box);
    CHECK(queries == 1 && generation > 0);
    CHECK(!callback_failed);
    CHECK(XmAutoSuggestBoxGetSuggestionCount(box) == 2);
    CHECK(XmAutoSuggestBoxSetSuggestions(box, NULL, 0, generation - 1) == False);
    CHECK(XmAutoSuggestBoxGetSuggestionCount(box) == 2);

    XmAutoSuggestBoxCommit(box, 1, NULL);
    value = XmSearchBoxGetString(search);
    CHECK(commits == 1 && strcmp(last_id, "apricot") == 0 && value != NULL &&
          strcmp(value, "Apricot") == 0);
    XtFree(value);
    CHECK(dismissals == 1);

    XmSearchBoxSetString(search, "zz");
    generation = XmAutoSuggestBoxGetGeneration(box);
    XtVaSetValues(box, XmNautoSuggestNoResultsLabel, (char *)"No match", NULL);
    CHECK(XmAutoSuggestBoxSetSuggestions(box, NULL, 0, generation));
    CHECK(XmAutoSuggestBoxGetSuggestionCount(box) == 0);
    XmAutoSuggestBoxDismiss(box, NULL);
    CHECK(dismissals == 2);

    accessibility = XmAccessibilityCreate(shell, "AutoSuggestBoxTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, box);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_PANEL && info.action_count == 0 &&
          info.name != NULL && strcmp(info.name, "Suggestions") == 0);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);

    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS auto suggest box: %u checks\n", checks);
    return EXIT_SUCCESS;
}
