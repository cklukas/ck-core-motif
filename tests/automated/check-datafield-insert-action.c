/* Oversized insert-string action parameters must not overflow DataField. */
#include <Xm/DataF.h>
#include <Xm/RowColumn.h>
#include <X11/Intrinsic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row, field;
    XEvent event;
    char oversized[128];
    String params[1];
    Cardinal count = 1;
    char *value;

    memset(oversized, 'x', sizeof(oversized) - 1);
    oversized[sizeof(oversized) - 1] = '\0';
    params[0] = oversized;
    shell = XtVaAppInitialize(&app, "DataFieldInsertAction", NULL, 0,
                              &argc, argv, NULL, NULL);
    row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell, NULL);
    field = XtVaCreateManagedWidget("field", xmDataFieldWidgetClass, row, NULL);
    XmDataFieldSetString(field, "abc");
    XtRealizeWidget(shell);
    memset(&event, 0, sizeof(event));
    event.type = KeyPress;
    event.xkey.display = XtDisplay(field);
    event.xkey.window = XtWindow(field);
    XtCallActionProc(field, "insert-string", &event, params, count);
    value = XmDataFieldGetString(field);
    if (!value || strcmp(value, "abc") != 0) {
        fprintf(stderr, "oversized insert-string parameter changed DataField\n");
        XtFree(value);
        return 1;
    }
    XtFree(value);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS oversized DataField insert-string parameter");
    return 0;
}
