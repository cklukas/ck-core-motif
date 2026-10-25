#include <X11/IntrinsicP.h>
#include <Xm/Accessible.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Sidebar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks;
static unsigned toggles;
static Boolean last_expanded;

#define CHECK(x) do { \
    if (!(x)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); \
        return EXIT_FAILURE; \
    } \
    ++checks; \
} while (0)

static void toggled(Widget widget, XtPointer client, XtPointer call)
{
    XmSidebarCallbackStruct *callback = (XmSidebarCallbackStruct *)call;
    (void)widget;
    (void)client;
    ++toggles;
    last_expanded = callback ? callback->expanded : False;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, sidebar, content, label;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;
    XmString text;
    Dimension width;

    (void)argv;
    shell = XtVaAppInitialize(&app, "SidebarTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 360, XmNheight, 160, NULL);
    sidebar = XmVaCreateManagedSidebar(shell, (char *)"sidebar",
                                       XmNwidth, 240, XmNheight, 120, NULL);
    XtAddCallback(sidebar, XmNsidebarToggleCallback, toggled, NULL);
    content = XmSidebarGetContent(sidebar);
    CHECK(content != NULL);
    text = XmStringCreateLocalized((char *)"Sidebar content");
    label = XtVaCreateManagedWidget("label", xmLabelWidgetClass, content,
                                    XmNlabelString, text, NULL);
    XmStringFree(text);
    CHECK(label != NULL && XtParent(label) == content);
    CHECK(XmSidebarGetExpanded(sidebar));

    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);
    XtResizeWidget(sidebar, 240, 120, 0);
    XSync(XtDisplay(shell), False);
    CHECK(XtIsManaged(content));
    XtVaGetValues(sidebar, XmNwidth, &width, NULL);
    CHECK(width == 240);

    XmSidebarSetExpanded(sidebar, False);
    CHECK(!XmSidebarGetExpanded(sidebar) && toggles == 1 && !last_expanded);
    CHECK(!XtIsManaged(content));
    XtVaGetValues(sidebar, XmNwidth, &width, NULL);
    CHECK(width == 240);
    XmSidebarToggle(sidebar, NULL);
    CHECK(XmSidebarGetExpanded(sidebar) && toggles == 2 && last_expanded);
    CHECK(XtIsManaged(content));

    accessibility = XmAccessibilityCreate(shell, "SidebarTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, sidebar);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_PANEL && info.action_count == 1 &&
          info.name != NULL && strcmp(info.name, "Sidebar") == 0);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);

    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS sidebar: %u checks\n", checks);
    return EXIT_SUCCESS;
}
