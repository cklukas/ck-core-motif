#include <Xm/Accessible.h>
#include <Xm/Link.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks, activations;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } ++checks; } while (0)
static void activated(Widget widget, XtPointer closure, XtPointer data) {
    XmLinkCallbackStruct *callback = (XmLinkCallbackStruct *)data;
    (void)widget; (void)closure; ++activations;
    if (!callback || callback->reason != XmLINK_REASON_ACTIVATE ||
        !callback->target || strcmp(callback->target, "https://example.invalid") != 0)
        fprintf(stderr, "bad link callback\n");
}
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, row, link; XmString label;
    XmAccessibility *accessibility; XmAccessibleInfo info; XmAccessibleId id;
    (void)argv;
    shell = XtVaAppInitialize(&app, "LinkTest", NULL, 0, &argc, argv, NULL,
                              XmNwidth, 260, XmNheight, 80, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0); XtManageChild(row);
    label = XmStringCreateLocalized((char *)"Open");
    link = XmVaCreateManagedLink(row, (char *)"link", XmNlinkLabel, label,
                                 XmNlinkTarget, "https://example.invalid", NULL);
    XmStringFree(label); XtAddCallback(link, XmNlinkActivateCallback, activated, NULL);
    XtRealizeWidget(shell); XSync(XtDisplay(shell), False);
    XmLinkActivate(link, NULL); CHECK(activations == 1);
    { Boolean visited = False; XtVaGetValues(link, XmNlinkVisited, &visited, NULL); CHECK(visited); }
    accessibility = XmAccessibilityCreate(shell, "LinkTest"); CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, link);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_BUTTON && info.action_count == 1);
    XmAccessibleInfoClear(&info); CHECK(XmAccessibilityAction(accessibility, id, 0));
    CHECK(activations == 2); XmAccessibilityDestroy(accessibility);
    XtSetSensitive(link, False); XmLinkActivate(link, NULL); CHECK(activations == 2);
    XtDestroyWidget(shell); XtCloseDisplay(XtDisplay(shell)); XtDestroyApplicationContext(app);
    printf("PASS link: %u checks\n", checks); return EXIT_SUCCESS;
}
