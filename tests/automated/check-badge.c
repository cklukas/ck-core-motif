#include <Xm/Accessible.h>
#include <Xm/Badge.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>
static unsigned checks;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } ++checks; } while (0)
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, row, count, label; XmString text;
    XmAccessibility *accessibility; XmAccessibleInfo info; XmAccessibleId id;
    (void)argv;
    shell = XtVaAppInitialize(&app, "BadgeTest", NULL, 0, &argc, argv, NULL,
                              XmNwidth, 240, XmNheight, 80, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0); XtManageChild(row);
    count = XmVaCreateManagedBadge(row, (char *)"count", XmNbadgeCount, 123,
                                   XmNbadgeMaximum, 99, XmNbadgeSeverity,
                                   XmBADGE_WARNING, NULL);
    text = XmStringCreateLocalized((char *)"New");
    label = XmVaCreateManagedBadge(row, (char *)"label", XmNbadgeLabel, text,
                                   XmNbadgeSeverity, XmBADGE_INFO, NULL);
    XmStringFree(text);
    XtRealizeWidget(shell); XSync(XtDisplay(shell), False);
    CHECK(XmBadgeGetDisplayedCount(count) == 99);
    CHECK(XmBadgeGetDisplayedCount(label) == -1);
    accessibility = XmAccessibilityCreate(shell, "BadgeTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, count);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.has_value && info.current_value == 99 && info.value_text != NULL);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell); XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS badge: %u checks\n", checks); return EXIT_SUCCESS;
}
