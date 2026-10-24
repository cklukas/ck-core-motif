/* Invalid wide text through DataField's access-text trait must become empty. */
#include <Xm/AccTextT.h>
#include <Xm/DataF.h>
#include <Xm/RowColumn.h>
#include <Xm/TraitP.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", \
    __FILE__, __LINE__, #x); exit(1); } ++checks; } while (0)
static unsigned long checks;
extern wchar_t *XmDataFieldGetSelectionWcs(Widget);

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row, field;
    Display *display;
    XmAccessTextualTrait trait;
    wchar_t valid[] = {L'G', 0x00fc, L'n', L't', L'e', L'r', 0};
    wchar_t invalid[] = {0xd800, 0};
    char *value;

    CHECK(setlocale(LC_CTYPE, "C.UTF-8") != NULL);
    shell = XtVaAppInitialize(&app, "DataFieldWcs", NULL, 0, &argc, argv,
                             NULL, NULL);
    row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell, NULL);
    field = XtVaCreateManagedWidget("field", xmDataFieldWidgetClass, row, NULL);
    display = XtDisplay(shell);
    XtRealizeWidget(shell);
    trait = (XmAccessTextualTrait)XmeTraitGet((XtPointer)XtClass(field),
                                               XmQTaccessTextual);
    CHECK(trait != NULL && trait->setValue != NULL);
    CHECK(trait->preferredFormat(field) == XmFORMAT_MBYTE);

    trait->setValue(field, valid, XmFORMAT_WCS);
    value = XmDataFieldGetString(field);
    CHECK(strcmp(value, "G\303\274nter") == 0);
    XtFree(value);

    XmDataFieldSetSelection(field, 0, 6, CurrentTime);
    value = XmDataFieldGetSelection(field);
    CHECK(value != NULL && strcmp(value, "G\303\274nter") == 0);
    XtFree(value);
    {
        wchar_t *selection = XmDataFieldGetSelectionWcs(field);
        CHECK(selection != NULL && wcscmp(selection, valid) == 0);
        XtFree((char *)selection);
    }

    trait->setValue(field, invalid, XmFORMAT_WCS);
    value = XmDataFieldGetString(field);
    CHECK(strcmp(value, "") == 0);
    XtFree(value);

    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS DataField wide access-text conversion: %lu checks\n", checks);
    return 0;
}
