/* Verify WP-02 modern-light colors, metrics, focus and precedence. */
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
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
widget_pixel_matches(Widget widget, const char *spec)
{
    Display *display = XtDisplay(widget);
    Colormap colormap = DefaultColormapOfScreen(XtScreen(widget));
    Pixel pixel = 0;

    XtVaGetValues(widget, XmNbackground, &pixel, NULL);
    return color_matches(display, colormap, pixel, spec);
}

static int
widget_foreground_matches(Widget widget, const char *spec)
{
    Display *display = XtDisplay(widget);
    Colormap colormap = DefaultColormapOfScreen(XtScreen(widget));
    Pixel pixel = 0;

    XtVaGetValues(widget, XmNforeground, &pixel, NULL);
    return color_matches(display, colormap, pixel, spec);
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
    Widget shell, form, label, button, toggle, text, list, scrollbar;
    XmString item;
    XmAppearanceProfile profile;
    Pixel explicit_background;
    Arg button_args[1];
    Dimension highlight = 0, margin = 0, spacing = 0;
    int status = EXIT_FAILURE;

    shell = XtVaAppInitialize(&app, "ModernLightTest", NULL, 0,
                              &argc, argv, NULL,
                              XmNwidth, 640, XmNheight, 220,
                              NULL);
    display = XtDisplay(shell);
    XtVaSetValues(shell, XmNkeyboardFocusPolicy, XmEXPLICIT, NULL);

    if (!XmSetAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT)) {
        fprintf(stderr, "modern-light selection failed\n");
        goto done;
    }
    if (!XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_MODERN_LIGHT) {
        fprintf(stderr, "modern-light query failed\n");
        goto done;
    }
    if (XmSetAppearanceProfile(display, XmAPPEARANCE_CLASSIC)) {
        fprintf(stderr, "profile changed after selection\n");
        goto done;
    }

    explicit_background = WhitePixelOfScreen(XtScreen(shell));
    XtSetArg(button_args[0], XmNbackground, explicit_background);
    form = XmCreateForm(shell, "form", NULL, 0);
    label = XmCreateLabel(form, "label", NULL, 0);
    button = XmCreatePushButton(form, "button", button_args, 1);
    toggle = XmCreateToggleButton(form, "toggle", NULL, 0);
    text = XmCreateTextField(form, "text", NULL, 0);
    list = XmCreateList(form, "list", NULL, 0);
    scrollbar = XmCreateScrollBar(form, "scrollbar", NULL, 0);

    XtVaSetValues(label,
                  XmNtopAttachment, XmATTACH_FORM,
                  XmNleftAttachment, XmATTACH_FORM,
                  NULL);
    XtVaSetValues(button,
                  XmNtopAttachment, XmATTACH_FORM,
                  XmNleftAttachment, XmATTACH_WIDGET,
                  XmNleftWidget, label,
                  NULL);
    XtVaSetValues(toggle,
                  XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, label,
                  XmNleftAttachment, XmATTACH_FORM,
                  NULL);
    XtVaSetValues(text,
                  XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, toggle,
                  XmNleftAttachment, XmATTACH_FORM,
                  NULL);
    XtVaSetValues(list,
                  XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, text,
                  XmNleftAttachment, XmATTACH_FORM,
                  XmNvisibleItemCount, 2,
                  NULL);
    XtVaSetValues(scrollbar,
                  XmNtopAttachment, XmATTACH_WIDGET,
                  XmNtopWidget, text,
                  XmNleftAttachment, XmATTACH_WIDGET,
                  XmNleftWidget, list,
                  XmNrightAttachment, XmATTACH_FORM,
                  NULL);
    item = XmStringCreateLocalized("modern-light");
    XmListAddItem(list, item, 0);
    XmStringFree(item);

    XtManageChildren((Widget[]){label, button, toggle, text, list, scrollbar},
                     6);
    XtManageChild(form);
    XtRealizeWidget(shell);
    XSync(display, False);
    drain_events(app);

    {
        int label_background = widget_pixel_matches(label, "#f6f8fb");
        int label_foreground = widget_foreground_matches(label, "#202124");
        int explicit_color = color_matches(
            display, DefaultColormapOfScreen(XtScreen(shell)),
            explicit_background, "#ffffff");
        if (!label_background || !label_foreground || !explicit_color) {
            fprintf(stderr, "modern-light or explicit colors were not applied\n");
            goto done;
        }
    }
    if (!XtIsRealized(form) || XtWindow(form) == None) {
        fprintf(stderr, "modern-light fixture did not realize its form window\n");
        goto done;
    }

    XtVaGetValues(button, XmNhighlightThickness, &highlight,
                  XmNmarginWidth, &margin, NULL);
    if (highlight != 2 || margin != 10 ||
        !widget_pixel_matches(button, "#ffffff") ||
        !widget_pixel_matches(toggle, "#fff4cc")) {
        fprintf(stderr, "button metrics or explicit color precedence failed\n");
        goto done;
    }
    XtVaGetValues(text, XmNmarginWidth, &margin, NULL);
    XtVaGetValues(list, XmNlistSpacing, &spacing, NULL);
    XtVaGetValues(scrollbar, XmNhighlightThickness, &highlight, NULL);
    if (margin != 8 || spacing != 2 || highlight != 2) {
        fprintf(stderr, "modern-light control metrics failed\n");
        goto done;
    }

    XmProcessTraversal(text, XmTRAVERSE_CURRENT);
    XSync(display, False);
    drain_events(app);
    if (XmGetFocusWidget(shell) != text) {
        fprintf(stderr, "keyboard focus did not reach the text field\n");
        goto done;
    }
    status = EXIT_SUCCESS;

done:
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    if (status == EXIT_SUCCESS)
        puts("PASS WP-02 modern-light profile");
    return status;
}
