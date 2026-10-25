/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/ComboBox.h>
#include <Xm/CascadeB.h>
#include <Xm/FileSB.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
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
    Widget shell, row, menu, pulldown, cascade, menu_item;
    Widget label, compound_label, list, combo, files;
    XmString items[3], selected = NULL, expected_label, expected_combo;
    XmString expected_menu, expected_file, compound;
    const char *values[] = {
        "e\314\201clair",       /* combining acute */
        "\360\237\230\200 cafe", /* non-BMP emoji */
        "\344\270\255\346\226\207" /* Han */
    };
    char *label_text = NULL;
    int *positions = NULL, position_count = 0;
    int *matches = NULL, match_count = 0;
    XmStringContext context;
    char *segment_text = NULL;
    XmStringCharSet segment_charset = NULL;
    XmStringDirection segment_direction;
    Boolean segment_separator;
    unsigned int segment_count = 0;

    CHECK(setlocale(LC_ALL, "C.UTF-8") != NULL);
    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "NonTextUnicodeRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    items[0] = XmStringCreateLocalized((char *)values[0]);
    items[1] = XmStringCreateLocalized((char *)values[1]);
    items[2] = XmStringCreateLocalized((char *)values[2]);
    expected_label = XmStringCreateLocalized((char *)values[1]);
    expected_combo = XmStringCreateLocalized((char *)values[2]);
    expected_menu = XmStringCreateLocalized((char *)values[0]);
    expected_file = XmStringCreateLocalized((char *)values[2]);
    compound = XmStringConcatAndFree(
        XmStringSegmentCreate((char *)values[0], XmFONTLIST_DEFAULT_TAG,
                              XmSTRING_DIRECTION_L_TO_R, True),
        XmStringSegmentCreate((char *)values[1], XmFONTLIST_DEFAULT_TAG,
                              XmSTRING_DIRECTION_L_TO_R, False));

    row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell,
                                  XmNorientation, XmVERTICAL, NULL);
    menu = XmCreateMenuBar(row, "menu", NULL, 0);
    XtManageChild(menu);
    pulldown = XmCreatePulldownMenu(menu, "pulldown", NULL, 0);
    menu_item = XtVaCreateManagedWidget("menu-item", xmPushButtonWidgetClass,
                                       pulldown, XmNlabelString, expected_menu,
                                       NULL);
    cascade = XtVaCreateManagedWidget("cascade", xmCascadeButtonWidgetClass,
                                      menu, XmNsubMenuId, pulldown, NULL);
    (void)cascade;

    label = XtVaCreateManagedWidget("label", xmLabelWidgetClass, shell,
                                   XmNlabelString, items[1], NULL);
    compound_label = XtVaCreateManagedWidget("compound-label",
                                             xmLabelWidgetClass, shell,
                                             XmNlabelString, compound, NULL);
    list = XtVaCreateManagedWidget("list", xmListWidgetClass, shell,
                                  XmNitems, items, XmNitemCount, 3,
                                  XmNselectionPolicy, XmBROWSE_SELECT, NULL);
    combo = XmVaCreateManagedComboBox(shell, "combo",
                                      XmNitems, items, XmNitemCount, 3,
                                      NULL);
    files = XmCreateFileSelectionBox(shell, "files", NULL, 0);
    XtManageChild(files);
    for (unsigned int i = 0; i < XtNumber(items); ++i)
        XmStringFree(items[i]);

    XtRealizeWidget(shell);
    XtVaGetValues(label, XmNlabelString, &selected, NULL);
    CHECK(selected != NULL);
    CHECK(XmStringGetLtoR(selected, XmFONTLIST_DEFAULT_TAG, &label_text));
    CHECK(XmStringByteCompare(selected, expected_label));
    XtFree(label_text);

    XtVaGetValues(compound_label, XmNlabelString, &selected, NULL);
    CHECK(selected != NULL && XmStringByteCompare(selected, compound));
    CHECK(XmStringInitContext(&context, selected));
    while (XmStringGetNextSegment(context, &segment_text, &segment_charset,
                                  &segment_direction, &segment_separator)) {
        CHECK(segment_text != NULL && segment_charset != NULL);
        ++segment_count;
        XtFree(segment_text);
        segment_text = NULL;
        segment_charset = NULL;
    }
    XmStringFreeContext(context);
    CHECK(segment_count == 2);

    XmListSelectPos(list, 3, False);
    CHECK(XmListGetSelectedPos(list, &positions, &position_count));
    CHECK(position_count == 1 && positions[0] == 3);
    XtFree((char *)positions);
    CHECK(XmListItemExists(list, expected_label));
    CHECK(XmListItemPos(list, expected_label) == 2);
    CHECK(XmListGetMatchPos(list, expected_label, &matches, &match_count));
    CHECK(match_count == 1 && matches[0] == 2);
    XtFree((char *)matches);

    XmComboBoxSelectItem(combo, expected_combo);
    XtVaGetValues(combo, XmNselectedItem, &selected, NULL);
    CHECK(selected != NULL);
    CHECK(XmStringGetLtoR(selected, XmFONTLIST_DEFAULT_TAG, &label_text));
    CHECK(XmStringByteCompare(selected, expected_combo));
    XtFree(label_text);

    XtVaGetValues(menu_item, XmNlabelString, &selected, NULL);
    CHECK(selected != NULL && XmStringByteCompare(selected, expected_menu));

    XtVaSetValues(files, XmNdirSpec, expected_file, NULL);
    XtVaGetValues(files, XmNdirSpec, &selected, NULL);
    CHECK(selected != NULL && XmStringByteCompare(selected, expected_file));

    XmStringFree(expected_label);
    XmStringFree(expected_combo);
    XmStringFree(expected_menu);
    XmStringFree(expected_file);
    XmStringFree(compound);

    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS non-text Unicode label/compound/list/ComboBox/menu/file-selection matrix: %lu checks\n", checks);
    return 0;
}
