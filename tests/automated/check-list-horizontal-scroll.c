#include <X11/Intrinsic.h>
#include <Xm/List.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } } while (0)

static int check_range(Widget list, Widget scrollbar)
{
    int value, slider, increment, page, minimum, maximum;
    (void)list;
    XmScrollBarGetValues(scrollbar, &value, &slider, &increment, &page);
    XtVaGetValues(scrollbar, XmNminimum, &minimum, XmNmaximum, &maximum, NULL);
    CHECK(maximum >= minimum);
    CHECK(slider >= 1);
    CHECK(slider <= maximum - minimum);
    CHECK(increment >= 1 && page >= 1);
    CHECK(value >= minimum);
    CHECK(value <= maximum - slider);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Display *display;
    Widget shell, scrolled, list, scrollbar = NULL;
    XmString long_item, short_item;
    XmString long_items[3];

    shell = XtVaAppInitialize(&app, "ListScrollRangeTest", NULL, 0,
                              &argc, argv, NULL,
                              XmNwidth, 180, XmNheight, 80, NULL);
    display = XtDisplay(shell);
    scrolled = XmCreateScrolledWindow(shell, (char *)"scrolled", NULL, 0);
    XtManageChild(scrolled);
    list = XmCreateList(scrolled, (char *)"list", NULL, 0);
    long_item = XmStringCreateLocalized((char *)
        "A very long list item that forces horizontal scrolling across the viewport");
    long_items[0] = long_item;
    long_items[1] = XmStringCreateLocalized((char *)
        "Another long item used to exercise the scrollbar range after shrink");
    long_items[2] = XmStringCreateLocalized((char *)"A third long item");
    XmListAddItems(list, long_items, 3, 0);
    XtManageChild(list);
    XtRealizeWidget(shell);
    XtVaGetValues(list, XmNhorizontalScrollBar, &scrollbar, NULL);
    CHECK(scrollbar != NULL);
    XmListSetHorizPos(list, 1000000);
    XSync(XtDisplay(shell), False);
    CHECK(check_range(list, scrollbar) == EXIT_SUCCESS);

    XmListDeleteAllItems(list);
    short_item = XmStringCreateLocalized((char *)"short");
    XmListAddItem(list, short_item, 0);
    XmStringFree(short_item);
    XtVaSetValues(list, XmNlistMarginWidth, 0, NULL);
    XSync(XtDisplay(shell), False);
    CHECK(check_range(list, scrollbar) == EXIT_SUCCESS);

    XmStringFree(long_item);
    XmStringFree(long_items[1]);
    XmStringFree(long_items[2]);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    puts("PASS List horizontal scrollbar remains valid after shrink");
    return EXIT_SUCCESS;
}
