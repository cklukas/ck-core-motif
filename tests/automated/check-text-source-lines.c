/* Hard-line scans must read actual characters on either side of the gap. */
#include <Xm/Text.h>
#include <Xm/TextP.h>
#include <Xm/TextStrSoP.h>
#include <assert.h>
#include <locale.h>
#include <stdio.h>

static void verify(Widget text, XmTextPosition second)
{
    XmTextSource source = XmTextGetSource(text);
    assert(source->Scan(source, 0, XmSELECT_LINE, XmsdRight, 1, False) == 5);
    assert(source->Scan(source, 0, XmSELECT_LINE, XmsdRight, 1, True) == 6);
    assert(source->Scan(source, 6, XmSELECT_LINE, XmsdRight, 1, False) == second);
    assert(source->CountLines(source, 0, (unsigned long)XmTextGetLastPosition(text)) == 2);
}
int main(int argc, char **argv)
{
    XtAppContext app;
    assert(setlocale(LC_ALL, "") != NULL);
    XtSetLanguageProc(NULL, NULL, NULL);
    Widget shell = XtVaAppInitialize(&app, "SourceLines", NULL, 0, &argc, argv, NULL, NULL);
    Widget text = XtVaCreateManagedWidget("text", xmTextWidgetClass, shell,
        XmNeditMode, XmMULTI_LINE_EDIT, XmNvalue, "alpha\nbeta\nlast", NULL);
    verify(text, 10);
    XmTextReplace(text, 8, 8, "!");
    verify(text, 11);
    XmTextReplace(text, 8, 9, "");
    verify(text, 10);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS native text hard-line scans across UTF-8 storage and gap edits");
    return 0;
}
