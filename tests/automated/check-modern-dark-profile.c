/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Verify the opt-in modern-dark profile, disabled state and pixmap identity. */
#include <Xm/Appearance.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ScrollBar.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/Xm.h>
#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>

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
check_color(Widget widget, const char *resource, const char *spec)
{
    Pixel pixel = 0;
    XtVaGetValues(widget, resource, &pixel, NULL);
    return color_matches(XtDisplay(widget),
                         DefaultColormapOfScreen(XtScreen(widget)),
                         pixel, spec);
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

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, form, label, button, toggle, text, scrollbar;
    Display *display;
    XmAppearanceProfile profile;
    Pixmap pixmap, returned_pixmap;
    Pixel explicit_background;
    Dimension highlight;
    int failed = 0;

    shell = XtVaAppInitialize(&app, "ModernDarkTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 600, XmNheight, 220, NULL);
    display = XtDisplay(shell);
    if (!XmSetAppearanceProfile(display, XmAPPEARANCE_MODERN_DARK) ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_MODERN_DARK ||
        XmSetAppearanceProfile(display, XmAPPEARANCE_HIGH_CONTRAST)) {
        fprintf(stderr, "modern-dark selection contract failed\n");
        failed = 1;
        goto done;
    }
    form = XmCreateForm(shell, "form", NULL, 0);
    label = XmCreateLabel(form, "label", NULL, 0);
    explicit_background = WhitePixelOfScreen(XtScreen(shell));
    button = XtVaCreateWidget("button", xmPushButtonWidgetClass, form,
                              XmNbackground, explicit_background, NULL);
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
    pixmap = XCreatePixmap(display, RootWindowOfScreen(XtScreen(shell)),
                           8, 8, DefaultDepthOfScreen(XtScreen(shell)));
    XtVaSetValues(button, XmNlabelType, XmPIXMAP, XmNlabelPixmap, pixmap, NULL);
    XtManageChildren((Widget[]){label, button, toggle, text, scrollbar}, 5);
    XtManageChild(form);
    XtSetSensitive(toggle, False);
    XtRealizeWidget(shell);
    XSync(display, False);
    drain(app);
    {
        int background_ok = check_color(label, XmNbackground, "#202124");
        int foreground_ok = check_color(label, XmNforeground, "#f1f3f4");
        int insensitive_ok = check_color(toggle, XmNbottomShadowColor, "#9aa0a6");
        int explicit_ok = check_color(button, XmNbackground, "#ffffff");
        if (!background_ok || !foreground_ok || !insensitive_ok || !explicit_ok) {
            fprintf(stderr, "modern-dark colors: background=%d foreground=%d insensitive=%d explicit=%d\n",
                    background_ok, foreground_ok, insensitive_ok, explicit_ok);
            failed = 1;
        }
    }
    XtVaGetValues(button, XmNhighlightThickness, &highlight,
                  XmNlabelPixmap, &returned_pixmap, NULL);
    if (highlight != 1 || returned_pixmap != pixmap ||
        XtWindow(form) == None) {
        fprintf(stderr, "modern-dark focus, pixmap or realization failed\n");
        failed = 1;
    }
    if (!XmProcessTraversal(text, XmTRAVERSE_CURRENT)) {
        fprintf(stderr, "modern-dark focus traversal failed\n");
        failed = 1;
    } else {
        XSync(display, False);
        drain(app);
        if (XmGetFocusWidget(shell) != text) {
            fprintf(stderr, "modern-dark focus destination failed\n");
            failed = 1;
        }
    }
    XFreePixmap(display, pixmap);
done:
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    if (!failed)
        puts("PASS WP-03 modern-dark profile");
    return failed;
}
