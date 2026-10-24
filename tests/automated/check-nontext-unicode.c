/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/ComboBox.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/Xm.h>
#include <locale.h>
#include <stdio.h>

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, \
                                #condition); return 1; } \
    ++checks; \
} while (0)

static unsigned long checks;

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, label, list, combo;
    XmString items[3], selected = NULL, expected_label, expected_combo;
    const char *values[] = {
        "e\314\201clair",       /* combining acute */
        "\360\237\230\200 cafe", /* non-BMP emoji */
        "\344\270\255\346\226\207" /* Han */
    };
    char *label_text = NULL;
    int *positions = NULL, position_count = 0;

    CHECK(setlocale(LC_ALL, "C.UTF-8") != NULL);
    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "NonTextUnicodeRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    items[0] = XmStringCreateLocalized((char *)values[0]);
    items[1] = XmStringCreateLocalized((char *)values[1]);
    items[2] = XmStringCreateLocalized((char *)values[2]);
    expected_label = XmStringCreateLocalized((char *)values[1]);
    expected_combo = XmStringCreateLocalized((char *)values[2]);

    label = XtVaCreateManagedWidget("label", xmLabelWidgetClass, shell,
                                   XmNlabelString, items[1], NULL);
    list = XtVaCreateManagedWidget("list", xmListWidgetClass, shell,
                                  XmNitems, items, XmNitemCount, 3,
                                  XmNselectionPolicy, XmBROWSE_SELECT, NULL);
    combo = XmVaCreateManagedComboBox(shell, "combo",
                                      XmNitems, items, XmNitemCount, 3,
                                      NULL);
    for (unsigned int i = 0; i < XtNumber(items); ++i)
        XmStringFree(items[i]);

    XtRealizeWidget(shell);
    XtVaGetValues(label, XmNlabelString, &selected, NULL);
    CHECK(selected != NULL);
    CHECK(XmStringGetLtoR(selected, XmFONTLIST_DEFAULT_TAG, &label_text));
    CHECK(XmStringByteCompare(selected, expected_label));
    XtFree(label_text);

    XmListSelectPos(list, 3, False);
    CHECK(XmListGetSelectedPos(list, &positions, &position_count));
    CHECK(position_count == 1 && positions[0] == 3);
    XtFree((char *)positions);

    XmComboBoxSelectItem(combo, expected_combo);
    XtVaGetValues(combo, XmNselectedItem, &selected, NULL);
    CHECK(selected != NULL);
    CHECK(XmStringGetLtoR(selected, XmFONTLIST_DEFAULT_TAG, &label_text));
    CHECK(XmStringByteCompare(selected, expected_combo));
    XtFree(label_text);
    XmStringFree(expected_label);
    XmStringFree(expected_combo);

    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS non-text Unicode label/list/ComboBox matrix: %lu checks\n", checks);
    return 0;
}
