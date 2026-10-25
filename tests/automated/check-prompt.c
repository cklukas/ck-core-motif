#include <Xm/Accessible.h>
#include <Xm/Prompt.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } ++checks; } while (0)
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, row, prompt, field; XmString text; char *value;
    XmAccessibility *accessibility; XmAccessibleInfo info; XmAccessibleId id;
    (void)argv;
    shell = XtVaAppInitialize(&app, "PromptTest", NULL, 0, &argc, argv, NULL,
                              XmNwidth, 320, XmNheight, 90, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0); XtManageChild(row);
    text = XmStringCreateLocalized((char *)"Find items");
    prompt = XmVaCreateManagedPrompt(row, (char *)"prompt", XmNpromptText, text, NULL);
    XmStringFree(text);
    field = XmCreateTextField(row, (char *)"field", NULL, 0); XtManageChild(field);
    XmTextFieldSetString(field, (char *)"entered");
    XtRealizeWidget(shell); XSync(XtDisplay(shell), False);
    CHECK(XmPromptGetVisible(prompt));
    value = XmTextFieldGetString(field); CHECK(value && strcmp(value, "entered") == 0); XtFree(value);
    accessibility = XmAccessibilityCreate(shell, "PromptTest"); CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, prompt);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_LABEL && info.name != NULL); XmAccessibleInfoClear(&info);
    XmPromptSetVisible(prompt, False); CHECK(!XmPromptGetVisible(prompt));
    CHECK(XmAccessibilityQuery(accessibility, id, &info)); CHECK(info.name == NULL); XmAccessibleInfoClear(&info);
    value = XmTextFieldGetString(field); CHECK(value && strcmp(value, "entered") == 0); XtFree(value);
    XmAccessibilityDestroy(accessibility); XtDestroyWidget(shell); XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app); printf("PASS prompt: %u checks\n", checks); return EXIT_SUCCESS;
}
