#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <Xm/NumberBox.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks;
static unsigned changes;
static int callback_value;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return EXIT_FAILURE; \
    } \
    ++checks; \
} while (0)

static void changed(Widget widget, XtPointer closure, XtPointer data) {
    XmNumberBoxCallbackStruct *callback = (XmNumberBoxCallbackStruct *)data;
    (void)widget; (void)closure;
    ++changes; callback_value = callback->value;
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, board, number;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;
    Display *display;
    Dimension width, height;
    int minimum, maximum;

    shell = XtVaAppInitialize(&app, "NumberBoxTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 320, XmNheight, 100, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0);
    XtManageChild(board);
    number = XmVaCreateManagedNumberBox(
        board, (char *)"number", XmNnumberMinimum, -10,
        XmNnumberMaximum, 10, XmNnumberValue, 3, XmNnumberStep, 2,
        XmNnumberPageStep, 5, XmNnumberWrap, True,
        XmNwidth, 220, XmNheight, 40, NULL);
    XtAddCallback(number, XmNnumberValueChangedCallback, changed, NULL);
    XtRealizeWidget(shell);
    display = XtDisplay(shell);
    XSync(display, False);
    XtVaGetValues(number, XmNwidth, &width, XmNheight, &height, NULL);
    CHECK(width == 220 && height == 40);
    CHECK(XmIsNumberBox(number));
    XmNumberBoxGetRange(number, &minimum, &maximum);
    CHECK(minimum == -10 && maximum == 10);
    CHECK(XmNumberBoxGetValue(number) == 3);
    XmNumberBoxStep(number, 1, False, True);
    CHECK(XmNumberBoxGetValue(number) == 5 && changes == 1 && callback_value == 5);
    XmNumberBoxStep(number, -1, True, True);
    CHECK(XmNumberBoxGetValue(number) == 0 && changes == 2);
    XmNumberBoxSetValue(number, 100, True);
    CHECK(XmNumberBoxGetValue(number) == 10 && changes == 3);
    XmNumberBoxStep(number, 1, False, True);
    CHECK(XmNumberBoxGetValue(number) == -10 && changes == 4);
    XmNumberBoxSetRange(number, 2, 4, True);
    CHECK(XmNumberBoxGetValue(number) == 2 && changes == 5);
    CHECK(XmProcessTraversal(number, XmTRAVERSE_CURRENT));
    CHECK(XmGetFocusWidget(shell) == number);
    accessibility = XmAccessibilityCreate(shell, "NumberBoxTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, number);
    CHECK(id != XmACCESSIBLE_NONE);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.has_value && info.minimum_value == 2 &&
          info.maximum_value == 4 && info.current_value == 2);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(number);
    printf("PASS number box: %u checks\n", checks);
    return EXIT_SUCCESS;
}
