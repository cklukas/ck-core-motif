#include <Xm/Accessible.h>
#include <Xm/FormSection.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } ++checks; } while (0)
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, row, section, content, child; XmString header, footer; XmAccessibility *accessibility; XmAccessibleInfo info; XmAccessibleId id;
    Dimension width, height;
    (void)argv;
    shell = XtVaAppInitialize(&app, "FormSectionTest", NULL, 0, &argc, argv, NULL, XmNwidth, 320, XmNheight, 160, NULL);
    row = XtVaCreateWidget("row", xmRowColumnWidgetClass, shell, XmNresizeWidth, False, XmNwidth, 120, NULL); XtManageChild(row);
    header = XmStringCreateLocalized((char *)"Account"); footer = XmStringCreateLocalized((char *)"Required fields");
    section = XmVaCreateManagedFormSection(row, (char *)"section", XmNformSectionHeader, header, XmNformSectionFooter, footer, XmNformSectionSpacing, 3, NULL);
    XmStringFree(header); XmStringFree(footer);
    content = XmFormSectionGetContent(section); CHECK(content != NULL); CHECK(XmFormSectionGetHeader(section) != NULL); CHECK(XmFormSectionGetFooter(section) != NULL);
    { XmString child_label = XmStringCreateLocalized((char *)"Value");
      child = XtVaCreateManagedWidget("field", xmLabelWidgetClass, content, XmNlabelString, child_label, NULL);
      XmStringFree(child_label); }
    (void)child;
    XtRealizeWidget(shell); XSync(XtDisplay(shell), False);
    XtVaGetValues(section, XmNwidth, &width, XmNheight, &height, NULL); CHECK(width > 0 && height > 0);
    header = XmStringCreateLocalized((char *)"Profile");
    XtVaSetValues(section, XmNformSectionHeader, header, NULL); XmStringFree(header);
    accessibility = XmAccessibilityCreate(shell, "FormSectionTest"); CHECK(accessibility != NULL); id = XmAccessibilityWidget(accessibility, section);
    CHECK(XmAccessibilityQuery(accessibility, id, &info)); CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL && strcmp(info.name, "Profile") == 0); XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);
    XtVaSetValues(row, XmNwidth, 86, NULL); XSync(XtDisplay(shell), False); XtVaGetValues(section, XmNwidth, &width, XmNheight, &height, NULL); CHECK(width == 80 && height > 0);
    accessibility = XmAccessibilityCreate(shell, "FormSectionTest"); CHECK(accessibility != NULL); id = XmAccessibilityWidget(accessibility, section);
    CHECK(XmAccessibilityQuery(accessibility, id, &info)); CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL && strcmp(info.name, "Profile") == 0); XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility); XtDestroyWidget(shell); XtCloseDisplay(XtDisplay(shell)); XtDestroyApplicationContext(app);
    printf("PASS form section: %u checks\n", checks); return EXIT_SUCCESS;
}
