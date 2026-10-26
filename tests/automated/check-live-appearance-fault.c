/* Verify that appearance transaction staging failure leaves the display intact. */
#include <Xm/Appearance.h>
#include <Xm/PushB.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <signal.h>
#include <stdio.h>

static int
fail(const char *message)
{
    fprintf(stderr, "live appearance rollback: %s\n", message);
    return 1;
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
    Widget shell, ordinary;
    Display *display;
    Pixel light_background, dark_background;
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    Dimension comfortable_margin, actual_margin;

    shell = XtVaAppInitialize(&app, "LiveAppearanceRollback", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    if (!XmSetAppearanceProfile(display, XmAPPEARANCE_CLASSIC))
        return fail("classic profile selection failed");
    ordinary = XmCreatePushButton(shell, "ordinary", NULL, 0);
    XtManageChild(ordinary);
    XtRealizeWidget(shell);
    XSync(display, False);

    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT) ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_MODERN_LIGHT)
        return fail("initial live transition failed");
    XtVaGetValues(ordinary, XmNbackground, &light_background, NULL);

    if (raise(SIGUSR1) != 0)
        return fail("could not enable allocation fault");
    if (XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_DARK))
        return fail("staged allocation failure was reported as success");
    if (raise(SIGUSR2) != 0)
        return fail("could not disable allocation fault");
    if (!XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_MODERN_LIGHT)
        return fail("failed transition changed the selected profile");
    XtVaGetValues(ordinary, XmNbackground, &dark_background, NULL);
    if (dark_background != light_background ||
        light_background == named_pixel(display, "#202124"))
        return fail("failed transition changed widget resources");

    XtVaGetValues(ordinary, XmNmarginWidth, &comfortable_margin, NULL);
    if (raise(SIGUSR1) != 0)
        return fail("could not enable density allocation fault");
    if (XmApplyAppearanceDensity(display, XmAPPEARANCE_DENSITY_COMPACT))
        return fail("density staging failure was reported as success");
    if (raise(SIGUSR2) != 0)
        return fail("could not disable density allocation fault");
    if (!XmGetAppearanceDensity(display, &density) ||
        density != XmAPPEARANCE_DENSITY_COMFORTABLE)
        return fail("failed transaction changed density selection");
    XtVaGetValues(ordinary, XmNmarginWidth, &actual_margin, NULL);
    if (actual_margin != comfortable_margin)
        return fail("failed density transaction changed layout resources");
    if (!XmApplyAppearanceDensity(display, XmAPPEARANCE_DENSITY_COMPACT) ||
        !XmGetAppearanceDensity(display, &density) ||
        density != XmAPPEARANCE_DENSITY_COMPACT)
        return fail("density transition after rollback failed");
    XtVaGetValues(ordinary, XmNmarginWidth, &actual_margin, NULL);
    if (actual_margin != 8 || actual_margin == comfortable_margin)
        return fail("successful density transition did not update layout");

    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_DARK) ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_MODERN_DARK)
        return fail("transition after rollback failed");
    XtVaGetValues(ordinary, XmNbackground, &dark_background, NULL);
    if (dark_background == light_background)
        return fail("successful transition did not update the widget");

    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    puts("PASS WP-05.1 live appearance rollback");
    return 0;
}
