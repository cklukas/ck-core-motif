#include <Xm/XmAll.h>
#include <Xm/ColumnP.h>
#include <assert.h>
#include <stdio.h>
int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "ColumnLifetime", NULL, 0,
                                     &argc, argv, NULL, NULL);
    for (unsigned i = 0; i < 3; ++i) {
        Widget column = XtVaCreateManagedWidget("column", xmColumnWidgetClass, shell, NULL);
        XmString label = XmStringCreateLocalized("Field label");
        Widget field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, column,
                                               XmNentryLabelString, label, NULL);
        XmString actual = NULL;
        XtVaGetValues(field, XmNentryLabelString, &actual, NULL);
        assert(actual && XmStringCompare(actual, label));
        XmStringFree(actual); XmStringFree(label);
        if (i == 0) XtDestroyWidget(field);
        if (i == 1) {
            Widget internal_label = XmColumnC_label_widget(field);
            assert(internal_label);
            XtDestroyWidget(internal_label);
            assert(XmColumnC_label_widget(field) == NULL);
        }
        XtDestroyWidget(column);
    }
    XtDestroyWidget(shell);
    puts("PASS Column label lifetime");
    return 0;
}
