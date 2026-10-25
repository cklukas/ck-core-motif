#include <Xm/Accessible.h>
#include <Xm/Divider.h>
#include <Xm/IconLabel.h>
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
    Widget shell, row, icon_label, divider, vertical;
    XmString label;
    Dimension icon_width, icon_height, divider_width, divider_height;
    XmAccessibility *accessibility;
    (void)argv;

    shell = XtVaAppInitialize(&app, "IconLabelDividerTest", NULL, 0,
                              &argc, argv, NULL, XmNwidth, 360, XmNheight,
                              120, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    label = XmStringCreateLocalized((char *)"Account");
    icon_label = XmVaCreateManagedIconLabel(
        row, (char *)"icon", XmNiconLabelLabel, label,
        XmNiconLabelPixmap, XmUNSPECIFIED_PIXMAP, XmNiconLabelSpacing, 9,
        NULL);
    divider = XmVaCreateManagedDivider(
        row, (char *)"divider", XmNdividerLabel, label,
        XmNdividerThickness, 2, NULL);
    vertical = XmVaCreateManagedDivider(
        row, (char *)"vertical", XmNdividerOrientation, XmVERTICAL,
        XmNdividerThickness, 3, XmNwidth, 20, XmNheight, 50, NULL);
    XmStringFree(label);
    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);

    XtVaGetValues(icon_label, XmNwidth, &icon_width, XmNheight, &icon_height,
                  NULL);
    CHECK(icon_width > 40 && icon_height >= 20);
    XtVaGetValues(divider, XmNwidth, &divider_width, XmNheight, &divider_height,
                  NULL);
    CHECK(divider_width > divider_height);
    {
        unsigned char orientation;
        Dimension thickness;
        XtVaGetValues(vertical, XmNdividerOrientation, &orientation,
                      XmNdividerThickness, &thickness, NULL);
        CHECK(orientation == XmVERTICAL);
        CHECK(thickness == 3);
    }
    accessibility = XmAccessibilityCreate(shell, "IconLabelDividerTest");
    CHECK(accessibility != NULL);
    CHECK(check_name(accessibility, icon_label, "Account"));
    CHECK(check_name(accessibility, divider, "Account"));
    XmAccessibleInfo info;
    CHECK(XmAccessibilityQuery(accessibility,
                               XmAccessibilityWidget(accessibility, vertical),
                               &info));
    CHECK(info.role == XmACCESSIBLE_LABEL && info.name != NULL);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS icon-label-divider: %u checks\n", checks);
    return EXIT_SUCCESS;
}
