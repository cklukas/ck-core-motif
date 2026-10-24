/* Invalid key-select parameters must leave a Text widget stable. */
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <X11/Intrinsic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row, text;
    XEvent event;
    String params[] = { (String)"not-a-direction" };
    Cardinal count = 1;
    char *value;

    shell = XtVaAppInitialize(&app, "TextKeySelection", NULL, 0,
                              &argc, argv, NULL, NULL);
    row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell, NULL);
    text = XtVaCreateManagedWidget("text", xmTextWidgetClass, row, NULL);
    XmTextSetString(text, "abc");
    XtRealizeWidget(shell);
    memset(&event, 0, sizeof(event));
    event.type = KeyPress;
    event.xkey.display = XtDisplay(text);
    event.xkey.window = XtWindow(text);
    XtCallActionProc(text, "key-select", &event, params, count);
    value = XmTextGetString(text);
    if (!value || strcmp(value, "abc") != 0) {
        fprintf(stderr, "invalid key-select parameter changed text\n");
        XtFree(value);
        return 1;
    }
    XtFree(value);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS invalid Text key-select parameter");
    return 0;
}
