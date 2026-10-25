/* Installed-consumer proof for the live appearance transaction. */
#include <Xm/Appearance.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <stdio.h>

static int
fail(const char *message)
{
    fprintf(stderr, "live appearance: %s\n", message);
    return 1;
}

static void
value_changed(Widget widget, XtPointer closure, XtPointer call_data)
{
    int *count = (int *)closure;
    (void)widget;
    (void)call_data;
    ++*count;
}

static Pixel
named_pixel(Display *display, const char *name)
{
    XColor exact, screen;
    Colormap colormap = DefaultColormap(display, DefaultScreen(display));
    if (!XAllocNamedColor(display, colormap, name, &screen, &exact))
        return 0;
    return screen.pixel;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, ordinary, explicit, toggle;
    Display *display;
    Pixel baseline_background, light_background, contrast_background;
    Pixel dark_background, classic_background;
    Pixel ordinary_foreground, explicit_foreground;
    Pixel ordinary_expected, red;
    int callback_count = 0;
    XmAppearanceProfile profile;

    shell = XtVaAppInitialize(&app, "LiveAppearance", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    if (!XmSetAppearanceProfile(display, XmAPPEARANCE_CLASSIC))
        return fail("classic profile selection failed");

    ordinary_expected = named_pixel(display, "#00ff00");
    red = named_pixel(display, "#ff0000");
    ordinary = XmCreatePushButton(shell, "ordinary", NULL, 0);
    explicit = XmCreatePushButton(shell, "explicit", NULL, 0);
    toggle = XmCreateToggleButton(shell, "toggle", NULL, 0);
    XtVaSetValues(explicit, XmNforeground, red, NULL);
    XtAddCallback(toggle, XmNvalueChangedCallback, value_changed,
                  (XtPointer)&callback_count);
    XtManageChild(ordinary);
    XtManageChild(explicit);
    XtManageChild(toggle);
    XtRealizeWidget(shell);
    XSync(display, False);

    XtVaGetValues(ordinary, XmNbackground, &baseline_background, NULL);
    XtVaGetValues(ordinary, XmNforeground, &ordinary_foreground, NULL);
    XtVaGetValues(explicit, XmNforeground, &explicit_foreground, NULL);
    XmToggleButtonSetState(toggle, True, True);
    if (!XmToggleButtonGetState(toggle) || callback_count != 1)
        return fail("toggle state/callback setup failed");

    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT) ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_MODERN_LIGHT)
        return fail("modern-light transition failed");
    XtVaGetValues(ordinary, XmNbackground, &light_background, NULL);
    if (light_background == baseline_background)
        return fail("modern-light did not update a realized widget");
    XtVaGetValues(ordinary, XmNforeground, &ordinary_foreground, NULL);
    XtVaGetValues(explicit, XmNforeground, &explicit_foreground, NULL);
    if (ordinary_foreground != ordinary_expected ||
        explicit_foreground != red || !XmToggleButtonGetState(toggle) ||
        callback_count != 1)
        return fail("light transition changed owned state or precedence");

    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_DARK))
        return fail("modern-dark transition failed");
    XtVaGetValues(ordinary, XmNbackground, &dark_background, NULL);
    XtVaGetValues(ordinary, XmNforeground, &ordinary_foreground, NULL);
    XtVaGetValues(explicit, XmNforeground, &explicit_foreground, NULL);
    if (dark_background == light_background || ordinary_foreground != ordinary_expected ||
        explicit_foreground != red ||
        !XmToggleButtonGetState(toggle) || callback_count != 1)
        return fail("dark transition changed owned state or precedence");

    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_HIGH_CONTRAST) ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_HIGH_CONTRAST)
        return fail("high-contrast transition failed");
    XtVaGetValues(ordinary, XmNbackground, &contrast_background, NULL);
    XtVaGetValues(ordinary, XmNforeground, &ordinary_foreground, NULL);
    if (contrast_background != named_pixel(display, "#000000") ||
        ordinary_foreground != ordinary_expected ||
        explicit_foreground != red || !XmToggleButtonGetState(toggle) ||
        callback_count != 1)
        return fail("high-contrast transition changed owned state or precedence");

    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_CLASSIC))
        return fail("classic transition failed");
    XtVaGetValues(ordinary, XmNbackground, &classic_background, NULL);
    XtVaGetValues(ordinary, XmNforeground, &ordinary_foreground, NULL);
    if (classic_background != baseline_background ||
        ordinary_foreground != ordinary_expected ||
        !XmToggleButtonGetState(toggle) || callback_count != 1)
        return fail("classic transition did not restore the original value");

    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    puts("PASS WP-05.1 live appearance transaction");
    return 0;
}
