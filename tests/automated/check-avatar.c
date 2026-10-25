#include <Xm/Accessible.h>
#include <Xm/Avatar.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } ++checks; } while (0)

static int check_name(XmAccessibility *accessibility, Widget widget,
                      const char *expected) {
    XmAccessibleInfo info;
    XmAccessibleId id = XmAccessibilityWidget(accessibility, widget);
    if (!XmAccessibilityQuery(accessibility, id, &info)) return 0;
    if (info.role != XmACCESSIBLE_LABEL || !info.name ||
        strcmp(info.name, expected) != 0) {
        XmAccessibleInfoClear(&info);
        return 0;
    }
    XmAccessibleInfoClear(&info);
    return 1;
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, row, initials, fallback;
    XmString text;
    Dimension width, height;
    XtWidgetGeometry preferred;
    XtGeometryResult geometry_result;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    (void)argv;

    shell = XtVaAppInitialize(&app, "AvatarTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 220, XmNheight, 80, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    text = XmStringCreateLocalized((char *)"AB");
    initials = XmVaCreateManagedAvatar(row, (char *)"initials",
                                       XmNavatarInitials, text,
                                       XmNavatarSize, 40, NULL);
    fallback = XmVaCreateManagedAvatar(row, (char *)"fallback",
                                       XmNavatarPixmap, XmUNSPECIFIED_PIXMAP,
                                       XmNavatarSize, 28, NULL);
    XmStringFree(text);
    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);
    geometry_result = XtQueryGeometry(initials, NULL, &preferred);
    CHECK((geometry_result == XtGeometryAlmost || geometry_result == XtGeometryYes) &&
          preferred.width == 40 && preferred.height == 40);
    XtVaGetValues(initials, XmNwidth, &width, XmNheight, &height, NULL);
    CHECK(height == 40 && width >= 40);
    geometry_result = XtQueryGeometry(fallback, NULL, &preferred);
    CHECK((geometry_result == XtGeometryAlmost || geometry_result == XtGeometryYes) &&
          preferred.width == 28 && preferred.height == 28);
    XtVaGetValues(fallback, XmNwidth, &width, XmNheight, &height, NULL);
    CHECK(height == 28 && width >= 28);
    accessibility = XmAccessibilityCreate(shell, "AvatarTest");
    CHECK(accessibility != NULL);
    CHECK(check_name(accessibility, initials, "AB"));
    CHECK(check_name(accessibility, fallback, "Avatar"));
    CHECK(XmAccessibilityQuery(accessibility,
                               XmAccessibilityWidget(accessibility, fallback),
                               &info));
    CHECK(info.role == XmACCESSIBLE_LABEL && info.name != NULL);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS avatar: %u checks\n", checks);
    return EXIT_SUCCESS;
}
