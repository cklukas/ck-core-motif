/* Exercise the real demo startup and Mrm hierarchy, replacing only its loop. */
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <assert.h>
#include <Xm/ListP.h>
#include <Xm/XmStringI.h>
static void VerifyApplication(XtAppContext context);
#define XtAppMainLoop VerifyApplication
#define main PeriodicMain
#include "../../demos/programs/periodic/periodic.c"
#undef main
#undef XtAppMainLoop

static void VerifyApplication(XtAppContext context)
{
    const char *expected = getenv("PERIODIC_EXPECT");
    Boolean historical = !strcmp(expected, "historical");
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    XClassHint hint;
    XrmValue value;
    char *type;
    const char *names[] = {"*workArea", "*pushButton", "*menuBar", "*infoDialog", "*fileDialog"};
    assert(XmGetAppearanceProfile(XtDisplay(shell), &profile));
    assert(XmGetAppearanceDensity(XtDisplay(shell), &density));
    assert(profile == (historical || !strcmp(expected, "classic") ? XmAPPEARANCE_CLASSIC :
        !strcmp(expected, "dark") ? XmAPPEARANCE_MODERN_DARK :
        !strcmp(expected, "contrast") ? XmAPPEARANCE_HIGH_CONTRAST : XmAPPEARANCE_MODERN_LIGHT));
    assert(density == (!strcmp(expected, "compact") ? XmAPPEARANCE_DENSITY_COMPACT : XmAPPEARANCE_DENSITY_COMFORTABLE));
    XSync(XtDisplay(shell), False);
    assert(XGetClassHint(XtDisplay(shell), XtWindow(shell), &hint));
    assert(!strcmp(hint.res_class, historical ? HISTORICAL_APP_CLASS : APP_CLASS));
    XFree(hint.res_name); XFree(hint.res_class);
    assert(XtIsRealized(shell));
    for (Cardinal i = 0; i < XtNumber(names); ++i) {
        Widget widget = XtNameToWidget(shell, (char *)names[i]);
        assert(widget != NULL);
        if (i == 1) {
            XmRenderTable table;
            XmString label;
            Dimension width, height;
            XtVaGetValues(widget, XmNrenderTable, &table, XmNlabelString, &label, NULL);
            assert(table != NULL && label != NULL);
            XmStringExtent(table, label, &width, &height);
            assert(width > 0 && height > 0);
            XmStringFree(label);
        }
        if (profile != XmAPPEARANCE_CLASSIC) {
            Pixel background;
            Colormap colormap;
            XColor actual, wanted, exact;
            const char *color = profile == XmAPPEARANCE_MODERN_DARK ? "#202124" :
                profile == XmAPPEARANCE_HIGH_CONTRAST ? "#000000" :
                i == 1 ? "#e2e6eb" : "#f1f2f4";
            if (i == 1 && !strcmp(expected, "override")) color = "#006644";
            XtVaGetValues(widget, XmNbackground, &background, XmNcolormap, &colormap, NULL);
            actual.pixel = background;
            XQueryColor(XtDisplay(widget), colormap, &actual);
            assert(XAllocNamedColor(XtDisplay(widget), colormap, color, &wanted, &exact));
            if (actual.red != wanted.red || actual.green != wanted.green || actual.blue != wanted.blue) {
                fprintf(stderr, "FAIL %s background: %04x/%04x/%04x expected %s\n",
                    names[i], actual.red, actual.green, actual.blue, color);
                abort();
            }
        }
    }
    if (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK) {
        Widget title = XtNameToWidget(shell, "*titleLabel");
        Boolean traversal;
        Dimension highlight;
        assert(title != NULL);
        XtVaGetValues(title, XmNtraversalOn, &traversal, XmNhighlightThickness, &highlight, NULL);
        assert(!traversal && highlight == 0);
    }
    /* A legacy app-defaults file must not enter the modern resource class. */
    if (!historical)
        assert(!XrmGetResource(XrmGetDatabase(XtDisplay(shell)), "periodic.fontList",
                              APP_CLASS ".FontList", &type, &value));
    {
        Widget list = XmCreateScrolledList(XtNameToWidget(shell, "*workArea"), "geometryProbe", NULL, 0);
        XmListWidget native = (XmListWidget)list;
        XtWidgetGeometry request, reply;
        Dimension heights[] = {0, 1, 65535};
        native->primitive.shadow_thickness = 0;
        native->list.HighlightThickness = 0;
        native->list.margin_height = 0;
        native->list.spacing = 0;
        request.request_mode = CWWidth | CWHeight;
        request.width = 65535;
        request.height = 65535;
        for (Cardinal i = 0; i < XtNumber(heights); ++i) {
            native->list.MaxItemHeight = heights[i];
            (void)XtQueryGeometry(list, &request, &reply);
        }
        XtDestroyWidget(XtParent(list));
    }
    puts("PASS Periodic startup, UIL controls/menus/dialogs, overrides, bounded list queries");
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(context);
}

int main(int argc, char **argv)
{
    /* Exercise the sentinel before any other Motif call initializes its cache. */
    XmStringTag tag = _XmStringCacheTag("periodic-native-tag", XmSTRING_TAG_STRLEN);
    assert(tag != NULL && !strcmp(tag, "periodic-native-tag"));
    assert(_XmStringCacheTag("periodic-native-tag", 19) == tag);
    assert(_XmStringCacheTag(XmFONTLIST_DEFAULT_TAG, XmSTRING_TAG_STRLEN) != NULL);
    return PeriodicMain(argc, argv);
}
