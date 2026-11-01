/* SPDX-License-Identifier: LGPL-2.0-or-later
 * Interpose only the XIM lookup boundary to test valid and malformed commits
 * deterministically. This does not claim live input-method qualification.
 */
#define _GNU_SOURCE
#include <Xm/List.h>
#include <Xm/XmIm.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

static const char *commit;
static int notifications, bad_callback, expected_position;
static const char *expected_label;

static void selected(Widget widget, XtPointer closure, XtPointer call_data)
{
    XmListCallbackStruct *data = call_data;
    char *text = NULL;
    (void)widget; (void)closure;
    ++notifications;
    if (data == NULL || data->reason != XmCR_BROWSE_SELECT ||
        data->item_position != expected_position ||
        !XmStringGetLtoR(data->item, XmFONTLIST_DEFAULT_TAG, &text) ||
        text == NULL || strcmp(text, expected_label) != 0)
        bad_callback = 1;
    XtFree(text);
}

int XmImMbLookupString(Widget widget, XKeyPressedEvent *event, char *buffer,
                       int capacity, KeySym *keysym, int *status)
{
    size_t length = strlen(commit);
    (void)widget; (void)event; (void)keysym;
    if (length > (size_t)capacity) { *status = XBufferOverflow; return (int)length; }
    memcpy(buffer, commit, length);
    *status = XLookupChars;
    return (int)length;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, list;
    XmString items[9];
    const char *labels[] = {"alpha", "\303\251clair", "\346\235\261\344\272\254",
                            "\303", "\303\251cole", "e\314\201clair",
                            "\360\237\230\200 smile", "\327\251\327\234\327\225\327\235",
                            "\330\263\331\204\330\247\331\205"};
    const char *inputs[] = {"\303\251", "\303\251", "\346\235\261", "\377", "\303", "a",
                            "e\314\201", "\360\237\230\200", "\327\251", "\330\263"};
    int expected[] = {2, 5, 3, 3, 3, 1, 6, 7, 8, 9};
    unsigned int i;
    int failed = 0;
    XEvent event;
    Dl_info loaded;
    const char *library = getenv("CKMOTIF_EXPECT_LIBRARY");
    char *actual_path, *expected_path;
    if (!library || !dladdr((void *)XmListSelectPos, &loaded)) return 2;
    actual_path = realpath(loaded.dli_fname, NULL);
    expected_path = realpath(library, NULL);
    if (!actual_path || !expected_path || strcmp(actual_path, expected_path)) return 2;
    printf("Loaded library: %s\n", actual_path);
    free(actual_path); free(expected_path);
    if (!setlocale(LC_ALL, "")) return 2;
    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "ListUnicodeRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    for (i = 0; i < XtNumber(items); ++i)
        items[i] = XmStringCreateLocalized((char *)labels[i]);
    list = XtVaCreateManagedWidget("list", xmListWidgetClass, shell,
        XmNitems, items, XmNitemCount, XtNumber(items),
        XmNmatchBehavior, XmQUICK_NAVIGATE, XmNselectionPolicy, XmBROWSE_SELECT, NULL);
    for (i = 0; i < XtNumber(items); ++i) XmStringFree(items[i]);
    XtAddCallback(list, XmNbrowseSelectionCallback, selected, NULL);
    XtRealizeWidget(shell);
    memset(&event, 0, sizeof(event));
    event.xkey.type = KeyPress;
    event.xkey.display = XtDisplay(list);
    event.xkey.window = XtWindow(list);
    XmListSelectPos(list, 1, False);
    XmListSetKbdItemPos(list, 1);
    for (i = 0; i < XtNumber(inputs); ++i) {
        int *positions = NULL, count = 0;
        int before = notifications;
        expected_position = expected[i];
        expected_label = labels[expected[i]-1];
        commit = inputs[i];
        XtCallActionProc(list, "ListQuickNavigate", &event, NULL, 0);
        if (!XmListGetSelectedPos(list, &positions, &count) ||
            count != 1 || positions[0] != expected[i]) {
            fprintf(stderr, "Unicode navigation case %u: selection=%d expected=%d\n",
                    i, count ? positions[0] : 0, expected[i]);
            failed = 1;
        }
        if (bad_callback || notifications != before + (i != 3 && i != 4)) {
            fprintf(stderr, "Unicode navigation case %u: incorrect callback payload/count\n", i);
            failed = 1;
        }
        XtFree((char *)positions);
    }
    /* A cached first character must change after replacing the item. */
    items[0] = XmStringCreateLocalized("\316\251mega");
    XmListReplaceItemsPos(list, items, 1, 2);
    XmStringFree(items[0]);
    expected_position = 2;
    expected_label = "\316\251mega";
    commit = "\316\251";
    XtCallActionProc(list, "ListQuickNavigate", &event, NULL, 0);
    if (bad_callback || XmListGetKbdItemPos(list) != 2 || notifications != 9) {
        fprintf(stderr, "Replacement navigation retained stale character or callback state\n");
        failed = 1;
    }
    XmListDeleteAllItems(list);
    XtCallActionProc(list, "ListQuickNavigate", &event, NULL, 0);
    if (notifications != 9) {
        fprintf(stderr, "Empty-list navigation emitted a callback\n");
        failed = 1;
    }
    items[0] = XmStringCreateLocalized((char *)labels[6]);
    XmListAddItem(list, items[0], 0);
    XmStringFree(items[0]);
    expected_position = 1;
    expected_label = labels[6];
    commit = "\360\237\230\200";
    XtCallActionProc(list, "ListQuickNavigate", &event, NULL, 0);
    if (bad_callback || XmListGetKbdItemPos(list) != 1 || notifications != 10) {
        fprintf(stderr, "Repopulated-list navigation failed\n");
        failed = 1;
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    if (!failed) puts("PASS Unicode navigation, callbacks, replacement, empty/repopulated list and malformed commits");
    return failed;
}
