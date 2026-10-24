/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/TextP.h>
#include <Xm/TextFP.h>
#include <Xm/RowColumn.h>
#include <Xm/Display.h>
#include <fontconfig/fontconfig.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int assertions;
static void require(int condition, const char *message)
{
    ++assertions;
    if (!condition) { fprintf(stderr, "preedit state: %s\n", message); exit(1); }
}
static void retired(Widget widget, XtPointer pointer, XtPointer call)
{
    (void)widget; (void)call; *(Widget *)pointer = NULL;
}
int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row, text, field, owner;
    Display *display;
    XmTextWidget tw;
    XmTextFieldWidget tf;
    char *value;
    size_t repeat;
    require(setlocale(LC_ALL, "C.UTF-8") != NULL, "UTF8 locale unavailable");
    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "PreeditState", NULL, 0, &argc, argv, NULL, NULL);
    display = XtDisplay(shell); owner = XmGetXmDisplay(display);
    require(owner != NULL, "native Display owner");
    XtAddCallback(owner, XmNdestroyCallback, retired, &owner);
    row = XtVaCreateManagedWidget("controls", xmRowColumnWidgetClass, shell, NULL);
    text = XtVaCreateManagedWidget("text", xmTextWidgetClass, row, XmNvalue, "ab", NULL);
    field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, row, XmNvalue, "ab", NULL);
    XtRealizeWidget(shell);
    tw = (XmTextWidget)text; tf = (XmTextFieldWidget)field;
    require(tw->text.onthespot && tf->text.onthespot, "native composition records");
    require(!XmTextHasPreedit(NULL) && !XmTextHasPreedit(row), "unsupported query target");
    require(!XmTextHasPreedit(text) && !XmTextHasPreedit(field), "inactive native composition");
    /* A focused query regression controls the actual native state records.
     * It does not simulate an input method, replace XIM callbacks or claim
     * external service/composition acceptance. Zero-length active composition
     * and verification are independently meaningful states. */
    for (repeat = 0; repeat < 8; ++repeat) {
        tw->text.onthespot->under_preedit = True;
        require(XmTextHasPreedit(text) && !XmTextHasPreedit(field), "Text empty active preedit");
        require(tw->text.onthespot->under_preedit, "query modified Text composition");
        tw->text.onthespot->under_preedit = False;
        tw->text.onthespot->under_verify_preedit = True;
        require(XmTextHasPreedit(text), "Text preedit verification");
        tw->text.onthespot->under_verify_preedit = False;
        tf->text.onthespot->under_preedit = True;
        require(XmTextHasPreedit(field) && !XmTextHasPreedit(text), "TextField independent preedit");
        require(tf->text.onthespot->under_preedit, "query modified TextField composition");
        tf->text.onthespot->under_preedit = False;
        tf->text.onthespot->under_verify_preedit = True;
        require(XmTextHasPreedit(field), "TextField preedit verification");
        tf->text.onthespot->under_verify_preedit = False;
        require(!XmTextHasPreedit(text) && !XmTextHasPreedit(field), "retired composition remained active");
    }
    value = XmTextGetString(text); require(value && strcmp(value, "ab") == 0, "Text bytes changed"); XtFree(value);
    value = XmTextFieldGetString(field); require(value && strcmp(value, "ab") == 0, "TextField bytes changed"); XtFree(value);
    XtDestroyWidget(shell); if (owner) XtDestroyWidget(owner);
    XtCloseDisplay(display); XtDestroyApplicationContext(app); FcFini();
    printf("preedit state assertions=%u\n", assertions);
    return 0;
}
