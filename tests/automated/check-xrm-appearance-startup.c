/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* WP-45: Verify declarative Xrm startup appearance/density selection,
 * explicit C API override precedence, widget resource precedence, and
 * single-warning deterministic fallback for invalid resource strings. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <Xm/Appearance.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/ScrollBar.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/Xm.h>

static int profile_warnings = 0;
static int density_warnings = 0;

static void
warning_handler(String message)
{
    if (message == NULL)
        return;
    if (strstr(message, "appearanceProfile") != NULL)
        ++profile_warnings;
    if (strstr(message, "appearanceDensity") != NULL)
        ++density_warnings;
}

static int
color_matches(Display *display, Colormap colormap, Pixel pixel,
              const char *spec)
{
    XColor expected, actual;
    actual.pixel = pixel;
    if (!XParseColor(display, colormap, spec, &expected) ||
        !XQueryColor(display, colormap, &actual))
        return 0;
    return (expected.red >> 8) == (actual.red >> 8) &&
           (expected.green >> 8) == (actual.green >> 8) &&
           (expected.blue >> 8) == (actual.blue >> 8);
}

static int
check_widget_color(Widget widget, const char *resource, const char *spec)
{
    Pixel pixel = 0;
    XtVaGetValues(widget, resource, &pixel, NULL);
    return color_matches(XtDisplay(widget),
                         DefaultColormapOfScreen(XtScreen(widget)),
                         pixel, spec);
}

static double
widget_color_luma(Widget widget, const char *resource)
{
    Pixel pixel = 0;
    XColor actual;
    Display *display = XtDisplay(widget);
    Colormap colormap = DefaultColormapOfScreen(XtScreen(widget));
    XtVaGetValues(widget, resource, &pixel, NULL);
    actual.pixel = pixel;
    if (!XQueryColor(display, colormap, &actual))
        return -1.0;
    return (0.2126 * (double)actual.red +
            0.7152 * (double)actual.green +
            0.0722 * (double)actual.blue) / 65535.0;
}

static Pixel
widget_color_pixel(Widget widget, const char *resource)
{
    Pixel pixel = 0;
    XtVaGetValues(widget, resource, &pixel, NULL);
    return pixel;
}

static Pixel
alloc_color(Widget widget, const char *spec)
{
    XColor exact, screen;
    Display *display = XtDisplay(widget);
    Colormap colormap = DefaultColormapOfScreen(XtScreen(widget));
    if (!XAllocNamedColor(display, colormap, spec, &screen, &exact))
        return 0;
    return screen.pixel;
}

static void
drain(XtAppContext app)
{
    while (XtAppPending(app)) {
        XEvent event;
        XtAppNextEvent(app, &event);
        XtDispatchEvent(&event);
    }
}

static int
run_unmodified_consumer(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, form, label, button, explicit_button, toggle, text, list, scrollbar;
    Display *display;
    XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
    XmAppearanceDensity density = XmAPPEARANCE_DENSITY_COMFORTABLE;
    XmAppearanceProfile expected_profile = XmAPPEARANCE_CLASSIC;
    XmAppearanceDensity expected_density = XmAPPEARANCE_DENSITY_COMFORTABLE;
    const char *env_profile = getenv("EXPECT_PROFILE");
    const char *env_density = getenv("EXPECT_DENSITY");
    Pixel explicit_bg;
    Dimension btn_margin = 0, explicit_margin = 0, ind_size = 0;
    Dimension text_margin = 0, list_spacing = 0, highlight = 0;
    int status = EXIT_FAILURE;

    if (env_profile != NULL) {
        if (strcmp(env_profile, "modern-light") == 0)
            expected_profile = XmAPPEARANCE_MODERN_LIGHT;
        else if (strcmp(env_profile, "modern-dark") == 0)
            expected_profile = XmAPPEARANCE_MODERN_DARK;
        else if (strcmp(env_profile, "high-contrast") == 0)
            expected_profile = XmAPPEARANCE_HIGH_CONTRAST;
        else if (strcmp(env_profile, "classic") == 0)
            expected_profile = XmAPPEARANCE_CLASSIC;
        else {
            fprintf(stderr, "unknown EXPECT_PROFILE=%s\n", env_profile);
            return EXIT_FAILURE;
        }
    }
    if (env_density != NULL) {
        if (strcmp(env_density, "compact") == 0)
            expected_density = XmAPPEARANCE_DENSITY_COMPACT;
        else if (strcmp(env_density, "comfortable") == 0)
            expected_density = XmAPPEARANCE_DENSITY_COMFORTABLE;
        else {
            fprintf(stderr, "unknown EXPECT_DENSITY=%s\n", env_density);
            return EXIT_FAILURE;
        }
    }

    /* Unmodified Motif consumer: never calls XmSetAppearanceProfile or
     * XmSetAppearanceDensity. */
    shell = XtVaAppInitialize(&app, "XrmAppearanceConsumer", NULL, 0,
                              &argc, argv, NULL,
                              XmNwidth, 520, XmNheight, 260, NULL);
    display = XtDisplay(shell);
    explicit_bg = alloc_color(shell, "#112233");

    form = XmCreateForm(shell, "form", NULL, 0);
    label = XmCreateLabel(form, "label", NULL, 0);
    button = XmCreatePushButton(form, "button", NULL, 0);
    explicit_button = XtVaCreateWidget("explicitButton",
                                       xmPushButtonWidgetClass, form,
                                       XmNbackground, explicit_bg,
                                       XmNmarginWidth, 29,
                                       NULL);
    toggle = XmCreateToggleButton(form, "toggle", NULL, 0);
    text = XmCreateTextField(form, "textField", NULL, 0);
    list = XmCreateList(form, "list", NULL, 0);
    scrollbar = XmCreateScrollBar(form, "scrollBar", NULL, 0);

    XtManageChildren((Widget[]){label, button, explicit_button, toggle,
                                text, list, scrollbar}, 7);
    XtManageChild(form);
    XtRealizeWidget(shell);
    XSync(display, False);
    drain(app);

    if (!XmGetAppearanceProfile(display, &profile) ||
        profile != expected_profile) {
        fprintf(stderr, "expected profile %d, got %d\n",
                (int)expected_profile, (int)profile);
        goto done;
    }
    if (!XmGetAppearanceDensity(display, &density) ||
        density != expected_density) {
        fprintf(stderr, "expected density %d, got %d\n",
                (int)expected_density, (int)density);
        goto done;
    }

    /* Verify explicit widget arguments and ordinary widget X resources always
     * override declarative profile/density defaults. */
    XtVaGetValues(explicit_button, XmNmarginWidth, &explicit_margin, NULL);
    if (!check_widget_color(explicit_button, XmNbackground, "#112233") ||
        explicit_margin != 29) {
        fprintf(stderr, "explicit widget argument precedence failed\n");
        goto done;
    }
    if (!check_widget_color(toggle, XmNbackground, "#445566")) {
        fprintf(stderr, "ordinary application X resource precedence failed\n");
        goto done;
    }

    if (expected_profile != XmAPPEARANCE_CLASSIC) {
        const char *expected_bg =
            expected_profile == XmAPPEARANCE_MODERN_LIGHT ? "#f1f2f4" :
            expected_profile == XmAPPEARANCE_MODERN_DARK ? "#202124" : "#000000";
        const char *expected_fg =
            expected_profile == XmAPPEARANCE_MODERN_LIGHT ? "#202124" :
            expected_profile == XmAPPEARANCE_MODERN_DARK ? "#f1f3f4" : "#ffffff";
        Dimension expected_btn_margin =
            expected_density == XmAPPEARANCE_DENSITY_COMPACT ? 8 : 12;
        Dimension expected_ind_size =
            expected_density == XmAPPEARANCE_DENSITY_COMPACT ? 16 : 18;
        Dimension expected_text_margin =
            expected_density == XmAPPEARANCE_DENSITY_COMPACT ? 6 : 8;
        Dimension expected_list_spacing =
            expected_density == XmAPPEARANCE_DENSITY_COMPACT ? 2 : 4;

        if (!check_widget_color(label, XmNbackground, expected_bg) ||
            !check_widget_color(label, XmNforeground, expected_fg)) {
            fprintf(stderr, "realized label palette mismatch for profile %d\n",
                    (int)expected_profile);
            goto done;
        }
        XtVaGetValues(button, XmNmarginWidth, &btn_margin,
                      XmNhighlightThickness, &highlight, NULL);
        XtVaGetValues(toggle, XmNindicatorSize, &ind_size, NULL);
        XtVaGetValues(text, XmNmarginWidth, &text_margin, NULL);
        XtVaGetValues(list, XmNlistSpacing, &list_spacing, NULL);
        if (btn_margin != expected_btn_margin ||
            ind_size != expected_ind_size ||
            text_margin != expected_text_margin ||
            list_spacing != expected_list_spacing ||
            highlight < 2) {
            fprintf(stderr,
                    "density metrics mismatch: btn=%u ind=%u text=%u list=%u hl=%u\n",
                    (unsigned)btn_margin, (unsigned)ind_size,
                    (unsigned)text_margin, (unsigned)list_spacing,
                    (unsigned)highlight);
            goto done;
        }
        /* Calling XmSetAppearanceProfile after controls exist must fail if a
         * different profile is requested. */
        if (XmSetAppearanceProfile(display, XmAPPEARANCE_CLASSIC)) {
            fprintf(stderr, "XmSetAppearanceProfile succeeded after controls existed\n");
            goto done;
        }
    }

    status = EXIT_SUCCESS;

done:
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    return status;
}

static int
run_precedence_and_warnings(int argc, char **argv)
{
    XtAppContext app;
    Display *d_override = NULL, *d_invalid = NULL;
    Widget shell_override = NULL, btn_override = NULL;
    Widget shell_invalid = NULL, btn_invalid = NULL;
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    Dimension margin = 0;
    char *override_argv[] = {
        argv[0],
        (char *)"-xrm", (char *)"*appearanceProfile: modern_dark",
        (char *)"-xrm", (char *)"*appearanceDensity: compact",
        NULL
    };
    int override_argc = 5;
    char *invalid_argv[] = {
        argv[0],
        (char *)"-xrm", (char *)"*appearanceProfile: invalid_profile_name",
        (char *)"-xrm", (char *)"*appearanceDensity: invalid_density_name",
        NULL
    };
    int invalid_argc = 5;
    (void)argc;

    XtToolkitInitialize();
    app = XtCreateApplicationContext();
    XtAppSetWarningHandler(app, warning_handler);

    /* 1. Explicit C API calls before control creation must override Xrm. */
    d_override = XtOpenDisplay(app, NULL, "xrmOverride", "XrmOverride",
                               NULL, 0, &override_argc, override_argv);
    if (d_override == NULL) {
        fprintf(stderr, "cannot open d_override\n");
        XtDestroyApplicationContext(app);
        return EXIT_FAILURE;
    }
    shell_override = XtAppCreateShell("xrmOverride", "XrmOverride",
                                      applicationShellWidgetClass,
                                      d_override, NULL, 0);
    if (!XmGetAppearanceProfile(d_override, &profile) ||
        profile != XmAPPEARANCE_MODERN_DARK ||
        !XmGetAppearanceDensity(d_override, &density) ||
        density != XmAPPEARANCE_DENSITY_COMPACT) {
        fprintf(stderr, "declarative Xrm not visible before C API override\n");
        XtDestroyWidget(shell_override);
        XtCloseDisplay(d_override);
        XtDestroyApplicationContext(app);
        return EXIT_FAILURE;
    }
    if (!XmSetAppearanceProfile(d_override, XmAPPEARANCE_MODERN_LIGHT) ||
        !XmSetAppearanceDensity(d_override, XmAPPEARANCE_DENSITY_COMFORTABLE)) {
        fprintf(stderr, "explicit C API call failed to override declarative Xrm\n");
        XtDestroyWidget(shell_override);
        XtCloseDisplay(d_override);
        XtDestroyApplicationContext(app);
        return EXIT_FAILURE;
    }
    /* A second conflicting XmSetAppearanceProfile call after explicit C API
     * selection must be rejected. */
    if (XmSetAppearanceProfile(d_override, XmAPPEARANCE_CLASSIC)) {
        fprintf(stderr, "second conflicting XmSetAppearanceProfile succeeded\n");
        XtDestroyWidget(shell_override);
        XtCloseDisplay(d_override);
        XtDestroyApplicationContext(app);
        return EXIT_FAILURE;
    }
    btn_override = XmCreatePushButton(shell_override, "btn", NULL, 0);
    XtManageChild(btn_override);
    XtRealizeWidget(shell_override);
    XSync(d_override, False);
    XtVaGetValues(btn_override, XmNmarginWidth, &margin, NULL);
    if (!check_widget_color(btn_override, XmNbackground, "#e2e6eb") ||
        margin != 12) {
        fprintf(stderr, "explicit C API override did not take effect on widget\n");
        XtDestroyWidget(shell_override);
        XtCloseDisplay(d_override);
        XtDestroyApplicationContext(app);
        return EXIT_FAILURE;
    }
    XtDestroyWidget(shell_override);
    XtCloseDisplay(d_override);

    /* 2. Invalid Xrm strings must emit exactly one warning each and fall back
     * deterministically to classic / comfortable. */
    profile_warnings = 0;
    density_warnings = 0;
    d_invalid = XtOpenDisplay(app, NULL, "xrmInvalid", "XrmInvalid",
                              NULL, 0, &invalid_argc, invalid_argv);
    if (d_invalid == NULL) {
        fprintf(stderr, "cannot open d_invalid\n");
        XtDestroyApplicationContext(app);
        return EXIT_FAILURE;
    }
    shell_invalid = XtAppCreateShell("xrmInvalid", "XrmInvalid",
                                     applicationShellWidgetClass,
                                     d_invalid, NULL, 0);
    btn_invalid = XmCreatePushButton(shell_invalid, "btn", NULL, 0);
    XtManageChild(btn_invalid);
    XtRealizeWidget(shell_invalid);
    XSync(d_invalid, False);

    if (!XmGetAppearanceProfile(d_invalid, &profile) ||
        profile != XmAPPEARANCE_CLASSIC ||
        !XmGetAppearanceDensity(d_invalid, &density) ||
        density != XmAPPEARANCE_DENSITY_COMFORTABLE) {
        fprintf(stderr, "invalid Xrm did not fall back to classic/comfortable\n");
        XtDestroyWidget(shell_invalid);
        XtCloseDisplay(d_invalid);
        XtDestroyApplicationContext(app);
        return EXIT_FAILURE;
    }
    /* Querying again must not emit duplicate warnings. */
    (void)XmGetAppearanceProfile(d_invalid, &profile);
    (void)XmGetAppearanceDensity(d_invalid, &density);
    if (profile_warnings != 1 || density_warnings != 1) {
        fprintf(stderr, "expected 1 profile warning and 1 density warning, got %d and %d\n",
                profile_warnings, density_warnings);
        XtDestroyWidget(shell_invalid);
        XtCloseDisplay(d_invalid);
        XtDestroyApplicationContext(app);
        return EXIT_FAILURE;
    }

    XtDestroyWidget(shell_invalid);
    XtCloseDisplay(d_invalid);
    XtDestroyApplicationContext(app);
    return EXIT_SUCCESS;
}

static int
run_colorobj_appearance_interop(int argc, char **argv)
{
    Display *server = NULL, *display = NULL;
    Window owner;
    Colormap server_cmap;
    XColor c_bg, c_fg, c_ts, c_bs, c_sc, exact;
    char set[128], blob[1100];
    char *p;
    XtAppContext app;
    Widget shell, form, label, button, text, list, scrollbar, explicit_toggle;
    XmRenderTable rt_dark = NULL, rt_classic = NULL;
    Dimension btn_margin = 0;
    char *co_argv[] = {
        argv[0],
        (char *)"-xrm", (char *)"*appearanceProfile: modern-dark",
        (char *)"-xrm", (char *)"*Font: -dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*",
        (char *)"-xrm", (char *)"*FontList: -dt-interface system-medium-r-normal-m*-*-*-*-*-*-*-*-*:",
        (char *)"-xrm", (char *)"*buttonFontList: -dt-interface system-medium-r-normal-m*-*-*-*-*-*-*-*-*:",
        (char *)"-xrm", (char *)"*labelFontList: -dt-interface system-medium-r-normal-m*-*-*-*-*-*-*-*-*:",
        (char *)"-xrm", (char *)"*textFontList: -dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:",
        (char *)"-xrm", (char *)"*explicitToggle.background: #445566",
        NULL
    };
    int co_argc = 15;
    int status = EXIT_FAILURE;
    (void)argc;

    server = XOpenDisplay(NULL);
    if (server == NULL) {
        fprintf(stderr, "cannot open server display\n");
        return EXIT_FAILURE;
    }
    server_cmap = DefaultColormap(server, DefaultScreen(server));
    if (!XAllocNamedColor(server, server_cmap, "#d8c0a8", &c_bg, &exact) ||
        !XAllocNamedColor(server, server_cmap, "#112233", &c_fg, &exact) ||
        !XAllocNamedColor(server, server_cmap, "#eeddcc", &c_ts, &exact) ||
        !XAllocNamedColor(server, server_cmap, "#665544", &c_bs, &exact) ||
        !XAllocNamedColor(server, server_cmap, "#bba088", &c_sc, &exact)) {
        fprintf(stderr, "cannot allocate ColorObj test palette\n");
        XCloseDisplay(server);
        return EXIT_FAILURE;
    }
    snprintf(set, sizeof(set), "%lx_%lx_%lx_%lx_%lx_",
             (unsigned long)c_bg.pixel, (unsigned long)c_fg.pixel,
             (unsigned long)c_ts.pixel, (unsigned long)c_bs.pixel,
             (unsigned long)c_sc.pixel);
    p = blob;
    memcpy(p, "1_", 2);
    p += 2;
    for (int i = 0; i < 8; ++i) {
        size_t slen = strlen(set);
        memcpy(p, set, slen);
        p += slen;
    }
    *p++ = '1';

    owner = XCreateSimpleWindow(server, DefaultRootWindow(server),
                                0, 0, 1, 1, 0, 0, 0);
    XChangeProperty(server, owner,
                    XInternAtom(server, "SDT Pixel Set", False),
                    XA_STRING, 8, PropModeReplace,
                    (const unsigned char *)blob, (int)(p - blob));
    XSetSelectionOwner(server,
                       XInternAtom(server, "Customize Data:0", False),
                       owner, CurrentTime);
    XSync(server, False);

    shell = XtVaAppInitialize(&app, "ColorObjAppearance", NULL, 0,
                              &co_argc, co_argv, NULL,
                              XmNwidth, 480, XmNheight, 240, NULL);
    display = XtDisplay(shell);
    form = XmCreateForm(shell, "form", NULL, 0);
    label = XmCreateLabel(form, "label", NULL, 0);
    button = XmCreatePushButton(form, "button", NULL, 0);
    text = XmCreateTextField(form, "textField", NULL, 0);
    list = XmCreateList(form, "list", NULL, 0);
    scrollbar = XmCreateScrollBar(form, "scrollBar", NULL, 0);
    /* Emulate dtfile setting XmNforeground to background on its scrollbar. */
    XtVaSetValues(scrollbar, XmNforeground, c_bg.pixel, NULL);
    explicit_toggle = XmCreateToggleButton(form, "explicitToggle", NULL, 0);

    XtManageChildren((Widget[]){label, button, text, list, scrollbar, explicit_toggle}, 6);
    XtManageChild(form);
    XtRealizeWidget(shell);
    XSync(display, False);
    drain(app);

    /* 1. Startup with active ColorObj and modern-dark must harmonize ColorObj
     * surfaces into dark modern roles with elevated button cards while
     * preserving explicit widget Xrm overrides, upgrading -dt-interface fonts
     * to Xft, and applying modern geometry. */
    XtVaGetValues(label, XmNrenderTable, &rt_dark, NULL);
    XtVaGetValues(button, XmNmarginWidth, &btn_margin, NULL);
    {
        XmString s_ext = XmStringCreateLocalized((char *)"Workspace One");
        Dimension ext_w = 0, ext_h = 0;
        if (rt_dark != NULL)
            XmStringExtent(rt_dark, s_ext, &ext_w, &ext_h);
        XmStringFree(s_ext);
        if (ext_w == 0 || ext_h == 0) {
            fprintf(stderr, "XmStringExtent with *Font Xrm + Xft renderTable failed\n");
            goto done;
        }
        /* Verify XmStringDraw with Xft + clip restores GC clip mask. */
        {
            Pixmap pm = XCreatePixmap(display, XtWindow(shell), 40, 20,
                                      DefaultDepth(display, DefaultScreen(display)));
            XGCValues gcv;
            GC gc;
            XRectangle clip_rect = {0, 0, 10, 10};
            XImage *im;
            Pixel px;
            gcv.foreground = c_bg.pixel;
            gc = XCreateGC(display, pm, GCForeground, &gcv);
            XFillRectangle(display, pm, gc, 0, 0, 40, 20);
            s_ext = XmStringCreateLocalized((char *)"ClipTest");
            XmStringDraw(display, pm, rt_dark, s_ext, gc, 0, 0, 10,
                         XmALIGNMENT_BEGINNING, XmSTRING_DIRECTION_L_TO_R,
                         &clip_rect);
            XmStringFree(s_ext);
            XSetForeground(display, gc, c_fg.pixel);
            XFillRectangle(display, pm, gc, 30, 10, 5, 5);
            im = XGetImage(display, pm, 32, 12, 1, 1, AllPlanes, ZPixmap);
            px = im != NULL ? XGetPixel(im, 0, 0) : 0;
            if (im != NULL)
                XDestroyImage(im);
            XFreeGC(display, gc);
            XFreePixmap(display, pm);
            if (px != c_fg.pixel) {
                fprintf(stderr, "XmStringDraw with Xft left clip mask stuck on GC\n");
                goto done;
            }
        }
    }
    if (widget_color_luma(label, XmNbackground) >= 0.25 ||
        widget_color_luma(label, XmNforeground) <= 0.80 ||
        widget_color_luma(button, XmNbackground) <=
            widget_color_luma(form, XmNbackground) ||
        widget_color_pixel(button, XmNbackground) ==
            widget_color_pixel(form, XmNbackground) ||
        !check_widget_color(explicit_toggle, XmNbackground, "#445566") ||
        rt_dark == NULL || btn_margin != 12) {
        fprintf(stderr, "ColorObj + modern-dark startup palette/metrics mismatch\n");
        goto done;
    }

    /* 2. Switching to modern-light must harmonize ColorObj surfaces into bright
     * modern light roles with elevated button cards while preserving explicit
     * widget overrides, modern typography, and geometry. */
    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT) ||
        widget_color_luma(label, XmNbackground) <= 0.85 ||
        widget_color_luma(label, XmNforeground) >= 0.20 ||
        widget_color_luma(button, XmNbackground) <= 0.78 ||
        widget_color_pixel(button, XmNbackground) ==
            widget_color_pixel(form, XmNbackground) ||
        !check_widget_color(explicit_toggle, XmNbackground, "#445566")) {
        fprintf(stderr, "ColorObj + modern-light switch palette mismatch\n");
        goto done;
    }

    /* 3. Switching back to classic must restore the exact baseline ColorObj
     * palette, companion shading colors, and -dt-interface render table. */
    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_CLASSIC) ||
        !check_widget_color(label, XmNbackground, "#d8c0a8") ||
        !check_widget_color(label, XmNforeground, "#112233") ||
        !check_widget_color(form, XmNtopShadowColor, "#eeddcc") ||
        !check_widget_color(form, XmNbottomShadowColor, "#665544") ||
        !check_widget_color(scrollbar, XmNtroughColor, "#bba088") ||
        !check_widget_color(explicit_toggle, XmNbackground, "#445566")) {
        fprintf(stderr, "ColorObj + classic restore palette mismatch\n");
        goto done;
    }
    XtVaGetValues(label, XmNrenderTable, &rt_classic, NULL);
    if (rt_classic == NULL || rt_classic == rt_dark) {
        fprintf(stderr, "ColorObj + classic restore renderTable mismatch\n");
        goto done;
    }

    status = EXIT_SUCCESS;

done:
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    XCloseDisplay(server);
    return status;
}

int
main(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "--check-precedence") == 0) {
        if (run_precedence_and_warnings(argc, argv) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        puts("PASS WP-45 Xrm appearance precedence and warning fallback");
        return EXIT_SUCCESS;
    }
    if (argc >= 2 && strcmp(argv[1], "--check-colorobj") == 0) {
        if (run_colorobj_appearance_interop(argc, argv) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        puts("PASS WP-45 ColorObj and modern appearance profile interop");
        return EXIT_SUCCESS;
    }
    if (run_unmodified_consumer(argc, argv) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    puts("PASS WP-45 unmodified Xrm appearance consumer");
    return EXIT_SUCCESS;
}
