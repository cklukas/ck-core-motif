#include <Xm/Container.h>
#include <Xm/IconG.h>
#include <Xm/TabList.h>
#include <X11/Intrinsic.h>
#include <math.h>
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
    Widget shell, first, second;
    XmString label;
    XmTabList first_tabs, second_tabs;
    XmTab first_tab, second_tab;
    float first_value, second_value;

    shell = XtVaAppInitialize(&app, "ContainerTabs", NULL, 0, &argc, argv,
                              NULL, NULL);
    first = XtVaCreateManagedWidget("first", xmContainerWidgetClass, shell,
                                   XmNwidth, 50000, XmNheight, 200,
                                   XmNlayoutType, XmDETAIL,
                                   XmNdetailOrderCount, 100,
                                   XmNspatialStyle, XmNONE, NULL);
    second = XtVaCreateManagedWidget("second", xmContainerWidgetClass, shell,
                                    XmNwidth, 50000, XmNheight, 200,
                                    XmNlayoutType, XmDETAIL,
                                    XmNdetailOrderCount, 250,
                                    XmNspatialStyle, XmNONE, NULL);
    label = XmStringCreateLocalized("item");
    XtVaCreateManagedWidget("first-item", xmIconGadgetClass, first,
                            XmNlabelString, label, NULL);
    XtVaCreateManagedWidget("second-item", xmIconGadgetClass, second,
                            XmNlabelString, label, NULL);
    XmStringFree(label);
    XtRealizeWidget(shell);
    XtVaGetValues(first, XmNdetailTabList, &first_tabs, NULL);
    XtVaGetValues(second, XmNdetailTabList, &second_tabs, NULL);
    CHECK(first_tabs != NULL && second_tabs != NULL);
    first_tab = XmTabListGetTab(first_tabs, 0);
    second_tab = XmTabListGetTab(second_tabs, 0);
    CHECK(first_tab != NULL && second_tab != NULL);
    first_value = XmTabGetValues(first_tab, NULL, NULL, NULL, NULL);
    second_value = XmTabGetValues(second_tab, NULL, NULL, NULL, NULL);
    CHECK(isfinite(first_value) && first_value > 0.0f);
    CHECK(isfinite(second_value) && second_value > 0.0f);
    XmTabFree(first_tab);
    XmTabFree(second_tab);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS container tab-list ownership: %lu checks\n", checks);
    return 0;
}
