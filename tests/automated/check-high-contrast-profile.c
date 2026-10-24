/* Verify WP-03 high-contrast colors, contrast ratios and precedence. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Appearance.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ScrollBar.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/Xm.h>

static double
linear_channel(unsigned int value)
{
    double channel = (double)value / 255.0;

    return channel <= 0.04045 ? channel / 12.92 :
           pow((channel + 0.055) / 1.055, 2.4);
}

static double
contrast_ratio(unsigned int first, unsigned int second)
{
    double first_luminance = 0.2126 * linear_channel((first >> 16) & 255) +
                             0.7152 * linear_channel((first >> 8) & 255) +
                             0.0722 * linear_channel(first & 255);
    double second_luminance = 0.2126 * linear_channel((second >> 16) & 255) +
                              0.7152 * linear_channel((second >> 8) & 255) +
                              0.0722 * linear_channel(second & 255);
    double brighter = first_luminance > second_luminance ? first_luminance :
                      second_luminance;
    double darker = first_luminance > second_luminance ? second_luminance :
                    first_luminance;

    return (brighter + 0.05) / (darker + 0.05);
}

static int
color_matches(Display *display, Colormap colormap, Pixel pixel,
              const char *spec)
{
    XColor expected;
    XColor actual;

    actual.pixel = pixel;
    if (!XParseColor(display, colormap, spec, &expected) ||
        !XQueryColor(display, colormap, &actual))
        return 0;
    return (expected.red >> 8) == (actual.red >> 8) &&
           (expected.green >> 8) == (actual.green >> 8) &&
           (expected.blue >> 8) == (actual.blue >> 8);
}

static int
widget_background_matches(Widget widget, const char *spec)
{
    Pixel pixel = 0;

    XtVaGetValues(widget, XmNbackground, &pixel, NULL);
    return color_matches(XtDisplay(widget),
                         DefaultColormapOfScreen(XtScreen(widget)),
                         pixel, spec);
}

static int
widget_foreground_matches(Widget widget, const char *spec)
{
    Pixel pixel = 0;

    XtVaGetValues(widget, XmNforeground, &pixel, NULL);
    return color_matches(XtDisplay(widget),
                         DefaultColormapOfScreen(XtScreen(widget)),
                         pixel, spec);
}

static void
drain_events(XtAppContext app)
{
    while (XtAppPending(app)) {
        XEvent event;
        XtAppNextEvent(app, &event);
        XtDispatchEvent(&event);
    }
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Display *display;
    Widget shell, form, label, button, toggle, text, scrollbar;
    Pixel explicit_background;
    Arg button_args[1];
    Dimension highlight = 0;
    XmAppearanceProfile profile;
    int status = EXIT_FAILURE;

    shell = XtVaAppInitialize(&app, "HighContrastTest", NULL, 0,
                              &argc, argv, NULL,
                              XmNwidth, 560, XmNheight, 180,
                              NULL);
    display = XtDisplay(shell);
    XtVaSetValues(shell, XmNkeyboardFocusPolicy, XmEXPLICIT, NULL);

    if (!XmSetAppearanceProfile(display, XmAPPEARANCE_HIGH_CONTRAST) ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_HIGH_CONTRAST ||
        XmSetAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT)) {
        fprintf(stderr, "high-contrast selection contract failed\n");
        goto done;
    }
    if (contrast_ratio(0xffffff, 0x000000) < 7.0 ||
        contrast_ratio(0xffff00, 0x000000) < 3.0 ||
        contrast_ratio(0xffffff, 0x0000ff) < 3.0 ||
        contrast_ratio(0xb3b3b3, 0x000000) < 4.5) {
        fprintf(stderr, "high-contrast palette failed contrast targets\n");
        goto done;
    }

    explicit_background = WhitePixelOfScreen(XtScreen(shell));
    XtSetArg(button_args[0], XmNbackground, explicit_background);
    form = XmCreateForm(shell, "form", NULL, 0);
    label = XmCreateLabel(form, "label", NULL, 0);
    button = XmCreatePushButton(form, "button", button_args, 1);
    toggle = XmCreateToggleButton(form, "toggle", NULL, 0);
    text = XmCreateTextField(form, "text", NULL, 0);
    scrollbar = XmCreateScrollBar(form, "scrollbar", NULL, 0);

    XtVaSetValues(label, XmNtopAttachment, XmATTACH_FORM,
                  XmNleftAttachment, XmATTACH_FORM, NULL);
    XtVaSetValues(button, XmNtopAttachment, XmATTACH_FORM,
                  XmNleftAttachment, XmATTACH_WIDGET,
                  XmNleftWidget, label, NULL);
    XtVaSetValues(toggle, XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, label, XmNleftAttachment, XmATTACH_FORM,
                  NULL);
    XtVaSetValues(text, XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, toggle, XmNleftAttachment, XmATTACH_FORM,
                  NULL);
    XtVaSetValues(scrollbar, XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, text, XmNleftAttachment, XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM, NULL);
    XtManageChildren((Widget[]){label, button, toggle, text, scrollbar}, 5);
    XtManageChild(form);
    XtRealizeWidget(shell);
    XSync(display, False);
    drain_events(app);

    if (!widget_background_matches(label, "#000000") ||
        !widget_foreground_matches(label, "#ffffff") ||
        !widget_background_matches(button, "#ffffff") ||
        !widget_background_matches(toggle, "#003300")) {
        fprintf(stderr, "high-contrast colors or precedence failed\n");
        goto done;
    }
    XtVaGetValues(button, XmNhighlightThickness, &highlight, NULL);
    if (highlight != 3 || !XtIsRealized(form) || XtWindow(form) == None) {
        fprintf(stderr, "high-contrast focus or realization failed\n");
        goto done;
    }
    if (!XmProcessTraversal(text, XmTRAVERSE_CURRENT)) {
        fprintf(stderr, "high-contrast focus traversal failed\n");
        goto done;
    }
    XSync(display, False);
    drain_events(app);
    if (XmGetFocusWidget(shell) != text) {
        fprintf(stderr, "high-contrast focus destination failed\n");
        goto done;
    }
    status = EXIT_SUCCESS;

done:
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    if (status == EXIT_SUCCESS)
        puts("PASS WP-03 high-contrast profile");
    return status;
}
