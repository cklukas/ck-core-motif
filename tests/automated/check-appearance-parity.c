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
    XmNframeShadowThickness, XmNborderWidth, XmNindicatorSize
};

static int compare_resources(Widget fresh, Widget live,
                             XtResourceList resources, Cardinal count)
{
    Cardinal i, j;
    for (i = 0; i < XtNumber(properties); ++i) {
        for (j = 0; j < count; ++j)
            if (!strcmp(resources[j].resource_name, properties[i])) {
                XtArgVal a = 0, b = 0;
                XtVaGetValues(fresh, properties[i], &a, NULL);
                XtVaGetValues(live, properties[i], &b, NULL);
                int same = a == b;
                if ((!strcmp(resources[j].resource_type, XmRPixel) ||
                     !strcmp(resources[j].resource_type, XmRSelectColor)) &&
                    (Pixel)a < XmHIGHLIGHT_COLOR && (Pixel)b < XmHIGHLIGHT_COLOR) {
                    XColor ca, cb;
                    Colormap ma, mb;
                    Widget wa = XtIsWidget(fresh) ? fresh : XtParent(fresh);
                    Widget wb = XtIsWidget(live) ? live : XtParent(live);
                    XtVaGetValues(wa, XmNcolormap, &ma, NULL);
                    XtVaGetValues(wb, XmNcolormap, &mb, NULL);
                    ca.pixel = a; cb.pixel = b;
                    XQueryColor(XtDisplay(wa), ma, &ca);
                    XQueryColor(XtDisplay(wb), mb, &cb);
                    same = ca.red == cb.red && ca.green == cb.green && ca.blue == cb.blue;
                }
                if (!same) {
                    fprintf(stderr, "FAIL %s.%s: startup=%lu live=%lu\n",
                            XtName(fresh), properties[i], (unsigned long)a,
                            (unsigned long)b);
                    return 0;
                }
                break;
            }
    }
    return 1;
}

static int compare(Widget fresh, Widget live)
{
    XtResourceList resources = NULL;
    XmSecondaryResourceData *secondary = NULL;
    Cardinal count = 0, blocks, i;
    int result;
    XtGetResourceList(XtClass(fresh), &resources, &count);
    result = compare_resources(fresh, live, resources, count);
    XtFree((char *)resources);
    blocks = XmGetSecondaryResourceData(XtClass(fresh), &secondary);
    for (i = 0; i < blocks; ++i) {
        if (!compare_resources(fresh, live, secondary[i]->resources,
                               secondary[i]->num_resources)) result = 0;
        XtFree((char *)secondary[i]->resources);
        XtFree((char *)secondary[i]);
    }
    XtFree((char *)secondary);
    return result;
}

int main(int argc, char **argv)
{
    XmAppearanceProfile target;
    XtAppContext app;
    Display *displays[3];
    Colormap colormaps[3];
    Widget shells[3], roots[3], widgets[3][13], kept[3];
    WidgetClass classes[] = {xmPushButtonWidgetClass, xmPushButtonGadgetClass,
        xmToggleButtonWidgetClass, xmLabelWidgetClass, xmLabelGadgetClass,
        xmTextFieldWidgetClass, xmTextWidgetClass, xmListWidgetClass,
        xmScrollBarWidgetClass, xmNotebookWidgetClass, xmToggleButtonGadgetClass,
        xmArrowButtonGadgetClass, xmSeparatorGadgetClass};
    Cardinal i, side;
    Pixel retained;
    Boolean overrides = argc >= 3;
    Boolean private_colormap = argc == 4;
    if (argc < 2 || argc > 4) return 2;
    target = !strcmp(argv[1], "light") ? XmAPPEARANCE_MODERN_LIGHT :
             !strcmp(argv[1], "dark") ? XmAPPEARANCE_MODERN_DARK :
             XmAPPEARANCE_HIGH_CONTRAST;
    argc = 1;
    XtToolkitInitialize(); app = XtCreateApplicationContext();
    for (side = 0; side < 3; ++side) {
        XrmDatabase db;
        displays[side] = XtOpenDisplay(app, NULL, "appearanceParity",
            "AppearanceParity", NULL, 0, &argc, argv);
        if (!displays[side]) return 3;
        db = XrmGetDatabase(displays[side]);
        XrmPutStringResource(&db, "AppearanceParity*kept.background", "#f1f2f4");
        if (overrides) {
            XrmPutStringResource(&db, "*background", "#e8e4dd");
            XrmPutStringResource(&db, "*marginWidth", "19");
            XrmPutStringResource(&db, "*shadowThickness", "5");
            XrmPutStringResource(&db, "*traversalOn", "True");
        }
        XrmSetDatabase(displays[side], db);
        if (!XmSetAppearanceProfile(displays[side], side ?
            XmAPPEARANCE_CLASSIC : target)) return 4;
        colormaps[side] = private_colormap ? XCreateColormap(displays[side],
            DefaultRootWindow(displays[side]), DefaultVisual(displays[side],
            DefaultScreen(displays[side])), AllocNone) :
            DefaultColormap(displays[side], DefaultScreen(displays[side]));
        shells[side] = XtVaAppCreateShell("appearanceParity", "AppearanceParity",
            applicationShellWidgetClass, displays[side],
            XtNvisual, DefaultVisual(displays[side], DefaultScreen(displays[side])),
            XmNdepth, DefaultDepth(displays[side], DefaultScreen(displays[side])),
            XmNcolormap, colormaps[side], NULL);
        roots[side] = XmCreateForm(shells[side], "root", NULL, 0);
        for (i = 0; i < XtNumber(classes); ++i) {
            char name[24]; snprintf(name, sizeof(name), "control%u", i);
            widgets[side][i] = XtCreateWidget(name, classes[i], roots[side], NULL, 0);
        }
        kept[side] = XmCreateLabel(roots[side], "kept", NULL, 0);
    }
    if (overrides) {
        Pixel background;
        Dimension margin, shadow;
        Boolean traversal;
        XColor exact, color;
        if (!XAllocNamedColor(displays[0], colormaps[0], "#e8e4dd", &color, &exact)) return 10;
        XtVaGetValues(widgets[0][0], XmNbackground, &background,
            XmNmarginWidth, &margin, XmNshadowThickness, &shadow, NULL);
        XtVaGetValues(widgets[0][3], XmNtraversalOn, &traversal, NULL);
        if (background != color.pixel || margin != 19 || shadow != 5 || !traversal) {
            XColor actual;
            Colormap shell_map, root_map, control_map;
            XtVaGetValues(shells[0], XmNcolormap, &shell_map, NULL);
            XtVaGetValues(roots[0], XmNcolormap, &root_map, NULL);
            XtVaGetValues(widgets[0][0], XmNcolormap, &control_map, NULL);
            fprintf(stderr, "maps requested=%lu shell=%lu root=%lu control=%lu\n",
                colormaps[0], shell_map, root_map, control_map);
            actual.pixel = background; XQueryColor(displays[0], colormaps[0], &actual);
            fprintf(stderr, "FAIL broad application resource precedence: pixel=%lu expected=%lu rgb=%04x/%04x/%04x expected-rgb=%04x/%04x/%04x margin=%u shadow=%u traversal=%u\n",
                background, color.pixel, actual.red, actual.green, actual.blue,
                color.red, color.green, color.blue, margin, shadow, traversal);
            return 11;
        }
    }
    {
        XrmDatabase db = XrmGetDatabase(displays[1]);
        XrmPutStringResource(&db, "AppearanceParity*late.background", "#007744");
        XrmSetDatabase(displays[1], db);
    }
    if (!XmApplyAppearanceProfile(displays[1], target)) return 5;
    {
        Widget late = XmCreatePushButton(roots[1], "late", NULL, 0);
        Pixel actual;
        XColor color;
        XtVaGetValues(late, XmNbackground, &actual, NULL);
        color.pixel = actual;
        XQueryColor(displays[1], colormaps[1], &color);
        if (color.red != 0 || color.green != 0x7777 || color.blue != 0x4444) {
            fputs("FAIL application database edit lost in transition\n", stderr); return 14;
        }
        XtDestroyWidget(late);
    }
    for (i = 0; i < XtNumber(classes); ++i)
        if (!compare(widgets[0][i], widgets[1][i])) return 6;
    if (!compare(kept[0], kept[1])) return 7;
    XtVaGetValues(kept[0], XmNbackground, &retained, NULL);
    if (!XmApplyAppearanceProfile(displays[0], XmAPPEARANCE_HIGH_CONTRAST) ||
        !XmApplyAppearanceProfile(displays[0], XmAPPEARANCE_MODERN_DARK)) return 8;
    { Pixel actual; XtVaGetValues(kept[0], XmNbackground, &actual, NULL);
      if (actual != retained) { fputs("FAIL explicit equal-value override lost\n", stderr); return 9; } }
    if (!XmApplyAppearanceProfile(displays[0], XmAPPEARANCE_CLASSIC)) return 12;
    for (i = 0; i < XtNumber(classes); ++i)
        if (!compare(widgets[2][i], widgets[0][i])) {
            fputs("FAIL modern-created classic restoration\n", stderr); return 13;
        }
    for (side = 0; side < 3; ++side) {
        XtDestroyWidget(shells[side]); XtCloseDisplay(displays[side]);
    }
    XtDestroyApplicationContext(app);
    puts("PASS appearance construction/live parity and equal-value override ownership");
    return 0;
}
