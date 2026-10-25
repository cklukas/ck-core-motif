#include <Xm/Accessible.h>
#include <Xm/EmptyState.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } ++checks; } while (0)
static void action(Widget widget, XtPointer data, XtPointer call) {
    unsigned *calls = (unsigned *)data; XmEmptyStateCallbackStruct *callback = (XmEmptyStateCallbackStruct *)call;
    (void)widget; if (!callback || callback->reason != XmCR_ACTIVATE) { *calls = 999; return; } ++*calls;
}
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, row, state, action_widget; XmString message, label; unsigned calls = 0; Boolean mapped;
    XmAccessibility *accessibility; XmAccessibleInfo info; XmAccessibleId id; Dimension width, height;
    (void)argv;
    shell = XtVaAppInitialize(&app, "EmptyStateTest", NULL, 0, &argc, argv, NULL, XmNwidth, 300, XmNheight, 160, NULL);
    row = XtVaCreateWidget("row", xmRowColumnWidgetClass, shell, XmNresizeWidth, False, XmNwidth, 180, NULL); XtManageChild(row);
    message = XmStringCreateLocalized((char *)"Nothing here"); label = XmStringCreateLocalized((char *)"Create item");
    state = XmVaCreateManagedEmptyState(row, (char *)"empty", XmNemptyStateMessage, message, XmNemptyStateActionLabel, label, NULL);
    XmStringFree(message); XmStringFree(label); XtAddCallback(state, XmNemptyStateActionCallback, action, &calls);
    CHECK(XmEmptyStateGetMessage(state) != NULL); action_widget = XmEmptyStateGetAction(state); CHECK(action_widget != NULL);
    XtRealizeWidget(shell); XSync(XtDisplay(shell), False);
    XtVaGetValues(state, XmNwidth, &width, XmNheight, &height, NULL); CHECK(width > 0 && height > 0); CHECK(XtIsManaged(action_widget));
    XmEmptyStateInvokeAction(state, NULL); CHECK(calls == 1);
    accessibility = XmAccessibilityCreate(shell, "EmptyStateTest"); CHECK(accessibility != NULL); id = XmAccessibilityWidget(accessibility, state);
    CHECK(XmAccessibilityQuery(accessibility, id, &info)); CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL && strcmp(info.name, "Nothing here") == 0 && info.action_count == 1); XmAccessibleInfoClear(&info);
    CHECK(XmAccessibilityAction(accessibility, id, 0)); CHECK(calls == 2); XmAccessibilityDestroy(accessibility);
    label = XmStringCreateLocalized((char *)""); XtVaSetValues(state, XmNemptyStateActionLabel, label, NULL); XmStringFree(label); XSync(XtDisplay(shell), False); XtVaGetValues(action_widget, XmNmappedWhenManaged, &mapped, NULL); CHECK(!mapped);
    XtDestroyWidget(shell); XtCloseDisplay(XtDisplay(shell)); XtDestroyApplicationContext(app);
    printf("PASS empty state: %u checks\n", checks); return EXIT_SUCCESS;
}
