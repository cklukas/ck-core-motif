/* Verify live appearance updates for gadget children and stale-state pruning. */
#include <Xm/Appearance.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <stdio.h>

static int
fail(const char *message)
{
    fprintf(stderr, "live appearance gadget: %s\n", message);
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
    Widget shell, row_column, gadget, replacement;
    Display *display;
    Pixel classic_background, light_background, dark_background;
    Pixel red;
    XmAppearanceProfile profile;

    shell = XtVaAppInitialize(&app, "LiveAppearanceGadget", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    if (!XmSetAppearanceProfile(display, XmAPPEARANCE_CLASSIC))
        return fail("classic profile selection failed");
    row_column = XmCreateRowColumn(shell, "rowColumn", NULL, 0);
    red = named_pixel(display, "#ff0000");
    gadget = XmCreatePushButtonGadget(row_column, "gadget", NULL, 0);
    XtVaSetValues(gadget, XmNforeground, red, NULL);
    replacement = NULL;
    XtManageChild(gadget);
    XtManageChild(row_column);
    XtRealizeWidget(shell);
    XSync(display, False);

    XtVaGetValues(gadget, XmNbackground, &classic_background, NULL);
    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT) ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_MODERN_LIGHT)
        return fail("modern-light transition failed");
    XtVaGetValues(gadget, XmNbackground, &light_background, NULL);
    if (light_background == classic_background)
        return fail("gadget background did not update");
    {
        Pixel foreground;
        XtVaGetValues(gadget, XmNforeground, &foreground, NULL);
        if (foreground != red)
            return fail("explicit gadget foreground was replaced");
    }

    /* A destroyed gadget has no widget destroy callback.  The next
     * transaction must prune its state from the parent manager before it is
     * considered for resource reads. */
    XtDestroyWidget(gadget);
    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_DARK) ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_MODERN_DARK)
        return fail("stale gadget state blocked the dark transition");

    replacement = XmCreatePushButtonGadget(row_column, "replacement", NULL, 0);
    XtManageChild(replacement);
    XtVaGetValues(replacement, XmNbackground, &dark_background, NULL);
    if (dark_background != named_pixel(display, "#202124"))
        return fail("replacement gadget did not inherit the active profile");
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    puts("PASS WP-05.1 live appearance gadget");
    return 0;
}
