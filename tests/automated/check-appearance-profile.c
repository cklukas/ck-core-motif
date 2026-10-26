/* Verify the WP-01 classic appearance-profile contract and precedence. */
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Appearance.h>
#include <Xm/Xm.h>

static int
shell_width(Widget shell)
{
    Dimension width = 0;

    XtVaGetValues(shell, XmNwidth, &width, NULL);
    return (int)width;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Display *display;
    Display *profile_display;
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    Widget shell;
    int status = EXIT_FAILURE;

    profile_display = XOpenDisplay(NULL);
    if (profile_display == NULL) {
        fprintf(stderr, "could not open profile display\n");
        return EXIT_FAILURE;
    }

    if (!XmGetAppearanceProfile(profile_display, &profile) ||
        profile != XmAPPEARANCE_CLASSIC) {
        fprintf(stderr, "unselected profile did not default to classic\n");
        XCloseDisplay(profile_display);
        return EXIT_FAILURE;
    }
    if (!XmSetAppearanceProfile(profile_display, XmAPPEARANCE_CLASSIC) ||
        !XmGetAppearanceProfile(profile_display, &profile) ||
        profile != XmAPPEARANCE_CLASSIC) {
        fprintf(stderr, "classic profile selection failed\n");
        XCloseDisplay(profile_display);
        return EXIT_FAILURE;
    }
    if (XmSetAppearanceProfile(profile_display, (XmAppearanceProfile)99)) {
        fprintf(stderr, "unknown profile was accepted\n");
        XCloseDisplay(profile_display);
        return EXIT_FAILURE;
    }
    if (!XmSetAppearanceDensity(profile_display, XmAPPEARANCE_DENSITY_COMPACT) ||
        !XmGetAppearanceDensity(profile_display, &density) ||
        density != XmAPPEARANCE_DENSITY_COMPACT) {
        fprintf(stderr, "density selection before Xt initialization failed\n");
        XCloseDisplay(profile_display);
        return EXIT_FAILURE;
    }
    XCloseDisplay(profile_display);

    shell = XtVaAppInitialize(&app, "AppearanceProfileTest", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);

    if (shell_width(shell) != 641) {
        fprintf(stderr, "resource precedence failed before explicit value: %d\n",
                shell_width(shell));
        goto done;
    }
    XtVaSetValues(shell, XmNwidth, 643, NULL);
    XtRealizeWidget(shell);
    XSync(display, False);

    if (shell_width(shell) != 643) {
        fprintf(stderr, "explicit value did not override resource: %d\n",
                shell_width(shell));
        goto done;
    }

    XtDestroyWidget(shell);
    status = EXIT_SUCCESS;

done:
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    if (status == EXIT_SUCCESS)
        puts("PASS WP-01 classic appearance profile");
    return status;
}
