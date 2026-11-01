#include <Xm/Accessible.h>
#include <Xm/Chip.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, activated, removed;

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return EXIT_FAILURE; } \
    ++checks; \
} while (0)

static void on_activate(Widget widget, XtPointer closure, XtPointer data) {
    XmChipCallbackStruct *callback = (XmChipCallbackStruct *)data;
    (void)widget; (void)closure;
    ++activated;
    if (!callback || callback->reason != XmCR_ACTIVATE)
        fprintf(stderr, "unexpected activation reason\n");
}

static void on_remove(Widget widget, XtPointer closure, XtPointer data) {
    XmChipCallbackStruct *callback = (XmChipCallbackStruct *)data;
    (void)closure;
    ++removed;
    if (!callback || callback->reason != XmCHIP_REASON_REMOVE)
        fprintf(stderr, "unexpected removal reason\n");
    (void)widget;
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, row, chip, doomed;
    XmString label;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;
    Dimension width = 0, height = 0;
    (void)argv;

    shell = XtVaAppInitialize(&app, "ChipTest", NULL, 0, &argc, argv, NULL,
                              XmNwidth, 260, XmNheight, 100, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    label = XmStringCreateLocalized((char *)"Filter");
    chip = XmVaCreateManagedChip(row, (char *)"chip", XmNchipLabel, label,
                                 XmNchipRemovable, True, NULL);
    XmStringFree(label);
    XtAddCallback(chip, XmNchipActivateCallback, on_activate, NULL);
    XtAddCallback(chip, XmNchipRemoveCallback, on_remove, NULL);
    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);
    XtVaGetValues(chip, XmNwidth, &width, XmNheight, &height, NULL);
    CHECK(width > 0 && height > 0);
    XmChipInvoke(chip, NULL);
    CHECK(activated == 1);
    accessibility = XmAccessibilityCreate(shell, "ChipTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, chip);
    CHECK(id != XmACCESSIBLE_NONE && XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_BUTTON && info.action_count == 2);
    XmAccessibleInfoClear(&info);
    CHECK(XmAccessibilityAction(accessibility, id, 0));
    CHECK(activated == 2);
    char *action=XmAccessibilityActionName(accessibility,id,1);
    CHECK(action && !strcmp(action,"remove"));XtFree(action);
    CHECK(XmAccessibilityAction(accessibility,id,1));CHECK(removed==1);
    XtVaSetValues(chip,XmNchipRemovable,False,NULL);
    CHECK(!XmAccessibilityAction(accessibility,id,1));
    CHECK(XmAccessibilityQuery(accessibility,id,&info) && info.action_count==1);
    XmAccessibleInfoClear(&info);
    XtVaSetValues(chip,XmNchipRemovable,True,NULL);
    XtSetSensitive(chip, False);
    XmChipInvoke(chip, NULL);
    CHECK(activated == 2);
    CHECK(!XmAccessibilityAction(accessibility,id,0));
    CHECK(!XmAccessibilityAction(accessibility,id,1));
    XtSetSensitive(chip, True);
    XmAccessibilityDestroy(accessibility);

    doomed = XmVaCreateManagedChip(row, (char *)"doomed", XmNchipRemovable,
                                   True, NULL);
    XtAddCallback(doomed, XmNchipRemoveCallback, on_remove, NULL);
    XmChipRemove(doomed, NULL);
    CHECK(removed == 2);
    XtDestroyWidget(doomed);
    Display *display=XtDisplay(shell);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS chip: %u checks\n", checks);
    return EXIT_SUCCESS;
}
