#include <Xm/Accessible.h>
#include <Xm/Toast.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } ++checks; } while (0)
static void action(Widget widget, XtPointer data, XtPointer call) { unsigned *calls = (unsigned *)data; XmToastCallbackStruct *callback = (XmToastCallbackStruct *)call; (void)widget; if (callback && callback->reason == XmCR_ACTIVATE) ++*calls; }
static void dismissed(Widget widget, XtPointer data, XtPointer call) { unsigned *calls = (unsigned *)data; XmToastCallbackStruct *callback = (XmToastCallbackStruct *)call; (void)widget; if (callback && callback->reason == XmCR_UNMAP) ++*calls; }
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, row, toast; XmString message, label; unsigned actions = 0, dismissals = 0; XmAccessibility *accessibility; XmAccessibleInfo info; XmAccessibleId id;
    (void)argv;
    shell = XtVaAppInitialize(&app, "ToastTest", NULL, 0, &argc, argv, NULL, XmNwidth, 320, XmNheight, 120, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0); XtManageChild(row);
    message = XmStringCreateLocalized((char *)"Saved"); label = XmStringCreateLocalized((char *)"Undo");
    toast = XmVaCreateToast(row, (char *)"toast", XmNtoastMessage, message, XmNtoastActionLabel, label, XmNtoastTimeout, 0UL, NULL);
    XmStringFree(message); XmStringFree(label); XtAddCallback(toast, XmNtoastActionCallback, action, &actions); XtAddCallback(toast, XmNtoastDismissCallback, dismissed, &dismissals);
    XtRealizeWidget(shell); XmToastShow(toast); XSync(XtDisplay(shell), False); CHECK(XmToastIsShown(toast));
    accessibility = XmAccessibilityCreate(shell, "ToastTest"); CHECK(accessibility != NULL); id = XmAccessibilityWidget(accessibility, toast);
    CHECK(XmAccessibilityQuery(accessibility, id, &info)); CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL && strcmp(info.name, "Saved") == 0 && info.action_count == 1); XmAccessibleInfoClear(&info);
    CHECK(XmAccessibilityAction(accessibility, id, 0)); CHECK(actions == 1); XmAccessibilityDestroy(accessibility);
    XmToastPause(toast, True); XmToastPause(toast, False); XmToastDismiss(toast, NULL); CHECK(!XmToastIsShown(toast)); CHECK(dismissals == 1);
    XtVaSetValues(toast, XmNtoastTimeout, 20UL, NULL); XmToastShow(toast); XtAppProcessEvent(app, XtIMTimer); CHECK(!XmToastIsShown(toast)); CHECK(dismissals == 2);
    XtDestroyWidget(shell); XtCloseDisplay(XtDisplay(shell)); XtDestroyApplicationContext(app);
    printf("PASS toast: %u checks\n", checks); return EXIT_SUCCESS;
}
