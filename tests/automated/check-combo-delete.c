/* Deleting a ComboBox selection must choose a surviving replacement. */
#include <Xm/ComboBox.h>
#include <Xm/List.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", \
    __FILE__, __LINE__, #x); fprintf(stderr, "%s\n", context); \
    exit(1); } ++checks; } while (0)
static unsigned long checks;
static char context[128];

typedef struct {
    unsigned calls;
    int position;
    XmString item;
} Expected;

static void selection(Widget widget, XtPointer closure, XtPointer data)
{
    Expected *expected = (Expected *)closure;
    XmComboBoxCallbackStruct *call = (XmComboBoxCallbackStruct *)data;
    (void)widget;
    CHECK(call != NULL);
    CHECK(call->reason == XmCR_SELECT);
    CHECK(call->event == NULL);
    CHECK(call->item_position == expected->position);
    CHECK(XmStringCompare(call->item_or_text, expected->item));
    ++expected->calls;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row;
    Display *display;
    unsigned type, mode, cases = 0;
    int count, selected, deleted, i;
    unsigned char types[] = {XmCOMBO_BOX, XmDROP_DOWN_COMBO_BOX,
                             XmDROP_DOWN_LIST};
    unsigned char modes[] = {XmZERO_BASED, XmONE_BASED};
    char *names[] = {"Alpha", "Bravo", "Charlie", "Delta", "Echo"};
    XmString items[XtNumber(names)];
    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "ComboDelete", NULL, 0, &argc, argv,
                             NULL, NULL);
    display = XtDisplay(shell);
    row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell, NULL);
    XtRealizeWidget(shell);
    for (i = 0; i < (int)XtNumber(items); ++i)
        items[i] = XmStringCreateLocalized(names[i]);
    for (type = 0; type < XtNumber(types); ++type) {
        for (mode = 0; mode < XtNumber(modes); ++mode) {
            for (count = 2; count <= (int)XtNumber(items); ++count) {
                for (selected = 1; selected <= count; ++selected) {
                    for (deleted = 0; deleted <= count; ++deleted) {
                        int removed = deleted ? deleted : count;
                        int next = selected == removed ?
                            (selected < count ? selected : count - 1) :
                            selected - (selected > removed);
                        int original = next + (next >= removed);
                        Expected expected = {0, next - (modes[mode] == XmZERO_BASED),
                                             items[original - 1]};
                        Widget list, text;
                        int actual_count, position, *positions = NULL, npositions;
                        XmStringTable actual_items;
                        XmString value;
                        char *visible;
                        snprintf(context, sizeof(context),
                            "type=%u mode=%u count=%d selected=%d delete=%d",
                            types[type], modes[mode], count, selected, deleted);
                        Widget combo = XtVaCreateManagedWidget("combo",
                            xmComboBoxWidgetClass, row,
                            XmNcomboBoxType, types[type],
                            XmNpositionMode, modes[mode],
                            XmNitems, items, XmNitemCount, count, NULL);
                        XmComboBoxSelectItem(combo, items[selected - 1]);
                        XtVaGetValues(combo, XmNlist, &list,
                                      XmNtextField, &text, NULL);
                        CHECK(list != NULL && text != NULL);
                        XtAddCallback(combo, XmNselectionCallback, selection,
                                      &expected);
                        XmComboBoxDeletePos(combo, deleted);
                        CHECK(expected.calls == (unsigned)(selected == removed));
                        XtVaGetValues(combo, XmNitemCount, &actual_count,
                            XmNselectedPosition, &position,
                            XmNselectedItem, &value, NULL);
                        CHECK(actual_count == count - 1);
                        CHECK(position == expected.position);
                        CHECK(XmStringCompare(value, expected.item));
                        XmStringFree(value);
                        visible = XmTextFieldGetString(text);
                        CHECK(strcmp(visible, names[original - 1]) == 0);
                        XtFree(visible);
                        CHECK(XmListGetSelectedPos(list, &positions, &npositions));
                        CHECK(npositions == 1);
                        CHECK(positions[0] == next);
                        XtFree((char *)positions);
                        XtVaGetValues(list, XmNitems, &actual_items,
                                      XmNitemCount, &actual_count, NULL);
                        CHECK(actual_count == count - 1);
                        for (i = 0; i < actual_count; ++i) {
                            int old = i + (i >= removed - 1);
                            CHECK(XmStringCompare(actual_items[i], items[old]));
                        }
                        XtDestroyWidget(combo);
                        ++cases;
                    }
                }
            }
        }
    }
    for (i = 0; i < (int)XtNumber(items); ++i) XmStringFree(items[i]);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS ComboBox deletion in %u cases: %lu checks\n", cases, checks);
    return 0;
}
