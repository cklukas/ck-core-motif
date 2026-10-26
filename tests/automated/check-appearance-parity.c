/* Native parity between construction-time and live profile resolution. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <X11/Shell.h>
#include <stdio.h>
#include <string.h>

static const char *properties[] = {
    XmNbackground, XmNforeground, XmNtopShadowColor, XmNbottomShadowColor,
    XmNhighlightColor, XmNhighlightThickness, XmNshadowThickness,
    XmNarmColor, XmNselectColor, XmNtraversalOn, XmNmarginWidth,
    XmNmarginHeight, XmNlistMarginWidth, XmNlistMarginHeight, XmNlistSpacing,
    XmNframeShadowThickness, XmNborderWidth
};

static int compare(Widget fresh, Widget live)
{
    XtResourceList resources = NULL;
    Cardinal count = 0, i, j;
    XtGetResourceList(XtClass(fresh), &resources, &count);
    for (i = 0; i < XtNumber(properties); ++i) {
        for (j = 0; j < count; ++j)
            if (!strcmp(resources[j].resource_name, properties[i])) {
                XtArgVal a = 0, b = 0;
                XtVaGetValues(fresh, properties[i], &a, NULL);
                XtVaGetValues(live, properties[i], &b, NULL);
                if (a != b) {
                    fprintf(stderr, "FAIL %s.%s: startup=%lu live=%lu\n",
                            XtName(fresh), properties[i], (unsigned long)a,
                            (unsigned long)b);
                    XtFree((char *)resources);
                    return 0;
                }
                break;
            }
    }
    XtFree((char *)resources);
    return 1;
}

int main(int argc, char **argv)
{
    XmAppearanceProfile target;
    XtAppContext app;
    Display *displays[2];
    Widget shells[2], roots[2], widgets[2][10], kept[2];
    WidgetClass classes[] = {xmPushButtonWidgetClass, xmPushButtonGadgetClass,
        xmToggleButtonWidgetClass, xmLabelWidgetClass, xmLabelGadgetClass,
        xmTextFieldWidgetClass, xmTextWidgetClass, xmListWidgetClass,
        xmScrollBarWidgetClass, xmNotebookWidgetClass};
    Cardinal i, side;
    Pixel retained;
    if (argc != 2) return 2;
    target = !strcmp(argv[1], "light") ? XmAPPEARANCE_MODERN_LIGHT :
             !strcmp(argv[1], "dark") ? XmAPPEARANCE_MODERN_DARK :
             XmAPPEARANCE_HIGH_CONTRAST;
    argc = 1;
    XtToolkitInitialize(); app = XtCreateApplicationContext();
    for (side = 0; side < 2; ++side) {
        XrmDatabase db;
        displays[side] = XtOpenDisplay(app, NULL, "appearanceParity",
            "AppearanceParity", NULL, 0, &argc, argv);
        if (!displays[side]) return 3;
        db = XrmGetDatabase(displays[side]);
        XrmPutStringResource(&db, "AppearanceParity*kept.background", "#f1f2f4");
        XrmSetDatabase(displays[side], db);
        if (!XmSetAppearanceProfile(displays[side], side ?
            XmAPPEARANCE_CLASSIC : target)) return 4;
        shells[side] = XtAppCreateShell("appearanceParity", "AppearanceParity",
            applicationShellWidgetClass, displays[side], NULL, 0);
        roots[side] = XmCreateForm(shells[side], "root", NULL, 0);
        for (i = 0; i < XtNumber(classes); ++i) {
            char name[24]; snprintf(name, sizeof(name), "control%u", i);
            widgets[side][i] = XtCreateWidget(name, classes[i], roots[side], NULL, 0);
        }
        kept[side] = XmCreateLabel(roots[side], "kept", NULL, 0);
    }
    if (!XmApplyAppearanceProfile(displays[1], target)) return 5;
    for (i = 0; i < XtNumber(classes); ++i)
        if (!compare(widgets[0][i], widgets[1][i])) return 6;
    if (!compare(kept[0], kept[1])) return 7;
    XtVaGetValues(kept[0], XmNbackground, &retained, NULL);
    if (!XmApplyAppearanceProfile(displays[0], XmAPPEARANCE_HIGH_CONTRAST) ||
        !XmApplyAppearanceProfile(displays[0], XmAPPEARANCE_MODERN_DARK)) return 8;
    { Pixel actual; XtVaGetValues(kept[0], XmNbackground, &actual, NULL);
      if (actual != retained) { fputs("FAIL explicit equal-value override lost\n", stderr); return 9; } }
    for (side = 0; side < 2; ++side) {
        XtDestroyWidget(shells[side]); XtCloseDisplay(displays[side]);
    }
    XtDestroyApplicationContext(app);
    puts("PASS appearance construction/live parity and equal-value override ownership");
    return 0;
}
