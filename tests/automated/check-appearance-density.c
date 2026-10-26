/* Native density contract: settings order, live parity, ownership and scale. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #test); return 1; } } while (0)

static XtSetValuesFunc saved_set_values;
static Display *nested_display;
static int nested_calls, nested_failures, callbacks;
static Boolean intercept(Widget old, Widget request, Widget current,
                          ArgList args, Cardinal *count)
{
    Boolean result = saved_set_values(old, request, current, args, count);
    if (nested_display) {
        ++nested_calls;
        if (XmApplyAppearanceDensity(nested_display, XmAPPEARANCE_DENSITY_COMFORTABLE) ||
            XmSetAppearanceDensity(nested_display, XmAPPEARANCE_DENSITY_COMPACT))
            ++nested_failures;
    }
    return result;
}
static void changed(Widget w, XtPointer closure, XtPointer data)
{ (void)w; (void)closure; (void)data; ++callbacks; }
static Dimension dimension(Widget w, const char *resource)
{ Dimension value = 0; XtVaGetValues(w, resource, &value, NULL); return value; }
static int equal(Widget a, Widget b)
{
    static const char *names[] = {XmNmarginWidth, XmNmarginHeight,
        XmNindicatorSize, XmNlistMarginWidth, XmNlistMarginHeight, XmNlistSpacing};
    XtResourceList list = NULL;
    Cardinal count, i, j;
    XtGetResourceList(XtClass(a), &list, &count);
    /* Gadget geometry is a public secondary resource, absent from Xt's
     * primary table. Assert it explicitly rather than silently skipping it. */
    if (XmIsLabelGadget(a)) {
        if (dimension(a, XmNmarginWidth) != dimension(b, XmNmarginWidth) ||
            dimension(a, XmNmarginHeight) != dimension(b, XmNmarginHeight)) {
            fputs("FAIL cached gadget margin parity\n", stderr);
            XtFree((char *)list); return 0;
        }
        if (XmIsToggleButtonGadget(a) &&
            dimension(a, XmNindicatorSize) != dimension(b, XmNindicatorSize)) {
            fputs("FAIL cached gadget indicator parity\n", stderr);
            XtFree((char *)list); return 0;
        }
    }
    for (i = 0; i < XtNumber(names); ++i)
        for (j = 0; j < count; ++j)
            if (!strcmp(list[j].resource_name, names[i])) {
                if (dimension(a, names[i]) != dimension(b, names[i])) {
                    fprintf(stderr, "FAIL %s.%s startup=%u live=%u\n", XtName(a),
                        names[i], dimension(a, names[i]), dimension(b, names[i]));
                    XtFree((char *)list); return 0;
                }
                break;
            }
    XtFree((char *)list); return 1;
}
int main(int argc, char **argv)
{
    XtAppContext app;
    Display *d[3];
    Widget shells[3], roots[3], controls[3][9], kept[3], explicit[3], late[3];
    Widget gadget_kept[3], gadget_explicit[3], gadget_late[3];
    WidgetClass classes[] = {xmPushButtonWidgetClass, xmPushButtonGadgetClass,
        xmToggleButtonWidgetClass, xmToggleButtonGadgetClass,
        xmTextFieldWidgetClass, xmTextWidgetClass, xmListWidgetClass,
        xmLabelWidgetClass, xmLabelGadgetClass};
    XmAppearanceProfile profile, selected;
    XmAppearanceDensity density;
    Cardinal side, i;
    Dimension classic_width, pixel;
    CoreClassPart *core;
    Boolean horizontal = argc == 3;
    CHECK(argc == 2 || argc == 3);
    profile = !strcmp(argv[1], "light") ? XmAPPEARANCE_MODERN_LIGHT :
        !strcmp(argv[1], "dark") ? XmAPPEARANCE_MODERN_DARK : XmAPPEARANCE_HIGH_CONTRAST;
    argc = 1;
    XtToolkitInitialize(); app = XtCreateApplicationContext();
    for (side = 0; side < 3; ++side) {
        XrmDatabase db;
        d[side] = XtOpenDisplay(app, NULL, "densityTest", "DensityTest", NULL, 0, &argc, argv);
        CHECK(d[side]);
        CHECK(XmGetAppearanceDensity(d[side], &density) && density == XmAPPEARANCE_DENSITY_COMFORTABLE);
        db = XrmGetDatabase(d[side]);
        XrmPutStringResource(&db, "DensityTest*kept.marginWidth", "12");
        XrmPutStringResource(&db, "DensityTest*keptGadget.marginWidth", "12");
        XrmSetDatabase(d[side], db);
        if (side == 0) {
            CHECK(XmSetAppearanceDensity(d[side], XmAPPEARANCE_DENSITY_COMPACT));
            CHECK(XmSetAppearanceProfile(d[side], profile));
        } else if (side == 1) {
            CHECK(XmSetAppearanceProfile(d[side], profile));
            CHECK(XmSetAppearanceDensity(d[side], XmAPPEARANCE_DENSITY_COMPACT));
            CHECK(XmSetAppearanceDensity(d[side], XmAPPEARANCE_DENSITY_COMFORTABLE));
        }
        shells[side] = XtAppCreateShell("densityTest", "DensityTest", applicationShellWidgetClass, d[side], NULL, 0);
        roots[side] = XmCreateRowColumn(shells[side], "root", NULL, 0);
        XtVaSetValues(roots[side], XmNorientation,
            horizontal ? XmHORIZONTAL : XmVERTICAL, NULL);
        XtManageChild(roots[side]);
        for (i = 0; i < XtNumber(classes); ++i) {
            char name[24]; snprintf(name, sizeof(name), "control%u", i);
            controls[side][i] = XtCreateManagedWidget(name, classes[i], roots[side], NULL, 0);
        }
        kept[side] = XmCreatePushButton(roots[side], "kept", NULL, 0);
        explicit[side] = XtVaCreateWidget("explicit", xmPushButtonWidgetClass,
            roots[side], XmNmarginWidth, 31, NULL);
        late[side] = XmCreatePushButton(roots[side], "late", NULL, 0);
        gadget_kept[side] = XmCreatePushButtonGadget(roots[side], "keptGadget", NULL, 0);
        gadget_explicit[side] = XtVaCreateWidget("explicitGadget", xmPushButtonGadgetClass,
            roots[side], XmNmarginWidth, 31, NULL);
        gadget_late[side] = XmCreatePushButtonGadget(roots[side], "lateGadget", NULL, 0);
        XtRealizeWidget(shells[side]);
        XSync(d[side], False);
    }
    CHECK(dimension(controls[0][0], XmNmarginWidth) == 8);
    CHECK(dimension(controls[1][0], XmNmarginWidth) == 12);
    CHECK(dimension(controls[0][2], XmNindicatorSize) == 16);
    CHECK(dimension(controls[1][2], XmNindicatorSize) == 18);
    CHECK(!XmSetAppearanceDensity(d[1], XmAPPEARANCE_DENSITY_COMPACT));
    CHECK(XmSetAppearanceDensity(d[1], XmAPPEARANCE_DENSITY_COMFORTABLE));
    XtVaSetValues(late[1], XmNmarginWidth, 12, NULL);
    XtVaSetValues(gadget_late[1], XmNmarginWidth, 12, NULL);
    XmToggleButtonSetState(controls[1][2], True, False);
    XmTextFieldSetString(controls[1][4], "kept text");
    XmTextFieldSetInsertionPosition(controls[1][4], 4);
    XtAddCallback(controls[1][2], XmNvalueChangedCallback, changed, NULL);
    XtAddCallback(controls[1][4], XmNvalueChangedCallback, changed, NULL);
    core = &xmPushButtonWidgetClass->core_class;
    saved_set_values = core->set_values; core->set_values = intercept;
    nested_display = d[1];
    CHECK(XmApplyAppearanceDensity(d[1], XmAPPEARANCE_DENSITY_COMPACT));
    nested_display = NULL; core->set_values = saved_set_values;
    CHECK(nested_calls > 0 && nested_failures == 0 && callbacks == 0);
    CHECK(XmToggleButtonGetState(controls[1][2]));
    { char *text = XmTextFieldGetString(controls[1][4]);
      CHECK(text && !strcmp(text, "kept text")); XtFree(text); }
    CHECK(XmTextFieldGetInsertionPosition(controls[1][4]) == 4);
    for (i = 0; i < XtNumber(classes); ++i) CHECK(equal(controls[0][i], controls[1][i]));
    CHECK(dimension(kept[1], XmNmarginWidth) == 12);
    CHECK(dimension(explicit[1], XmNmarginWidth) == 31);
    CHECK(dimension(late[1], XmNmarginWidth) == 12);
    CHECK(dimension(gadget_kept[1], XmNmarginWidth) == 12);
    CHECK(dimension(gadget_explicit[1], XmNmarginWidth) == 31);
    CHECK(dimension(gadget_late[1], XmNmarginWidth) == 12);
    CHECK(XmGetAppearanceProfile(d[1], &selected) && selected == profile);
    CHECK(XmApplyAppearanceProfile(d[1], XmAPPEARANCE_MODERN_DARK));
    CHECK(XmGetAppearanceDensity(d[1], &density) && density == XmAPPEARANCE_DENSITY_COMPACT);
    CHECK(dimension(controls[1][0], XmNmarginWidth) == 8);
    XtVaSetValues(late[1], XmNmarginWidth, 27, NULL);
    CHECK(XmApplyAppearanceDensity(d[1], XmAPPEARANCE_DENSITY_COMFORTABLE));
    CHECK(dimension(late[1], XmNmarginWidth) == 27);
    CHECK(dimension(controls[1][0], XmNmarginWidth) == 12);
    CHECK(dimension(controls[1][1], XmNmarginWidth) == 12);
    CHECK(dimension(kept[1], XmNmarginWidth) == 12);
    CHECK(dimension(explicit[1], XmNmarginWidth) == 31);
    classic_width = dimension(controls[2][0], XmNmarginWidth);
    CHECK(XmApplyAppearanceDensity(d[2], XmAPPEARANCE_DENSITY_COMPACT));
    CHECK(dimension(controls[2][0], XmNmarginWidth) == classic_width);
    CHECK(XmGetAppearanceProfile(d[2], &selected) && selected == XmAPPEARANCE_CLASSIC);
    CHECK(XmApplyAppearanceProfile(d[2], profile));
    CHECK(dimension(controls[2][0], XmNmarginWidth) == 8);
    CHECK(XmApplyAppearanceProfile(d[2], XmAPPEARANCE_CLASSIC));
    CHECK(dimension(controls[2][0], XmNmarginWidth) == classic_width);
    CHECK(!XmApplyAppearanceDensity(d[2], (XmAppearanceDensity)99));
    CHECK(XmGetAppearanceDensity(d[2], &density) && density == XmAPPEARANCE_DENSITY_COMPACT);
    CHECK(!XmGetAppearanceDensity(NULL, &density) && !XmGetAppearanceDensity(d[2], NULL));
    pixel = 77;
    CHECK(!XmGetAppearanceSpacing(d[2], (XmAppearanceSpacing)-1, &pixel) && pixel == 77);
    CHECK(!XmGetAppearanceSpacing(NULL, XmAPPEARANCE_SPACING_SMALL, &pixel) && pixel == 77);
    for (i = 0; i < 4; ++i) {
        static const double scales[] = {1.0, 1.5, 2.0, 3.0};
        static const Dimension base[] = {4, 8, 12, 16, 24};
        Cardinal j;
        CHECK(XmSetDisplayScaleOverride(d[2], scales[i]));
        for (j = 0; j < XtNumber(base); ++j) {
            CHECK(XmGetAppearanceSpacing(d[2], (XmAppearanceSpacing)j, &pixel));
            CHECK(pixel == (Dimension)(base[j] * scales[i] + 0.5));
        }
    }
    for (side = 0; side < 3; ++side) { XtDestroyWidget(shells[side]); XtCloseDisplay(d[side]); }
    XtDestroyApplicationContext(app);
    puts("PASS density order/parity, classic isolation, overrides, reentry, values and scaled spacing");
    return 0;
}
