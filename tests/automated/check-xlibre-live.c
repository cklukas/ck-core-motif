/* Exercise live XLibre output-property updates through the installed shell observer. */
#define _POSIX_C_SOURCE 200809L
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <Xm/Label.h>

#include "XlibreDpiI.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int callback_count;
static double callback_old_scale;
static double callback_new_scale;
static RROutput callback_old_output;
static RROutput callback_new_output;

static void
scale_changed(Widget shell, double old_scale, double new_scale,
              RROutput old_output, RROutput new_output, XtPointer closure)
{
    (void)shell;
    (void)closure;
    ++callback_count;
    callback_old_scale = old_scale;
    callback_new_scale = new_scale;
    callback_old_output = old_output;
    callback_new_output = new_output;
}

static int
fail(const char *message)
{
    fprintf(stderr, "XLibre live observer: %s\n", message);
    return EXIT_FAILURE;
}

static int
find_output_for_scale(Display *display, double scale, RROutput *output_return,
                      long *dpi_return)
{
    XRRScreenResources *resources;
    int i;

    resources = XRRGetScreenResourcesCurrent(display,
                                              DefaultRootWindow(display));
    if (resources == NULL)
        return 0;
    for (i = 0; i < resources->noutput; ++i) {
        XRROutputInfo *output_info = XRRGetOutputInfo(
            display, resources, resources->outputs[i]);
        double dpi;
        if (output_info == NULL)
            continue;
        if (output_info->connection == RR_Connected && output_info->crtc != None &&
            _XmXlibreDpiGetOutputDpi(display, resources->outputs[i], &dpi) &&
            fabs(dpi / _XM_XLIBRE_DPI_BASE - scale) < 0.000001) {
            *output_return = resources->outputs[i];
            *dpi_return = (long)(dpi + 0.5);
            XRRFreeOutputInfo(output_info);
            XRRFreeScreenResources(resources);
            return 1;
        }
        XRRFreeOutputInfo(output_info);
    }
    XRRFreeScreenResources(resources);
    return 0;
}

static int
wait_for_callback(XtAppContext app)
{
    struct timespec pause = { 0, 10000000L };
    struct timespec started;
    struct timespec now;

    if (clock_gettime(CLOCK_MONOTONIC, &started) != 0)
        return 0;
    for (;;) {
        if (callback_count != 0)
            return 1;
        if (XtAppPending(app))
            XtAppProcessEvent(app, XtIMAll);
        else
            nanosleep(&pause, NULL);
        if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
            return 0;
        if (now.tv_sec - started.tv_sec > 3 ||
            (now.tv_sec == started.tv_sec &&
             now.tv_nsec - started.tv_nsec > 3000000000L))
            return 0;
    }
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget child;
    Display *display;
    RROutput output;
    Atom dpi_atom;
    double shell_scale;
    double child_scale;
    double dpi;
    long old_dpi;
    long new_dpi;
    long property_value;
    int installed;
    int status = EXIT_SUCCESS;
    Boolean property_changed = False;
    Boolean observer_installed = False;

    shell = XtVaAppInitialize(&app, (String)"XlibreLive", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    if (!_XmXlibreDpiServerSupported(display)) {
        XtDestroyWidget(shell);
        XtDestroyApplicationContext(app);
        puts("SKIP XLibre live observer requires a real XLibre display");
        return 77;
    }
    child = XtVaCreateManagedWidget("child", xmLabelWidgetClass, shell, NULL);
    XtRealizeWidget(shell);
    installed = _XmXlibreDpiInstallShell(shell, scale_changed, NULL);
    if (!installed)
        status = fail("observer registration failed");
    else
        observer_installed = True;
    if (!installed)
        goto cleanup;
    if (!_XmXlibreDpiGetWidgetScale(shell, &shell_scale) ||
        !_XmXlibreDpiGetWidgetScale(child, &child_scale) ||
        fabs(shell_scale - child_scale) > 0.000001)
        {
            status = fail("shell and descendant did not share one effective scale");
            goto cleanup;
        }
    if (!find_output_for_scale(display, shell_scale, &output, &old_dpi))
        {
            status = fail("could not identify the shell's DPI output");
            goto cleanup;
        }

    if (old_dpi == 96)
        new_dpi = 144;
    else if (old_dpi == 144)
        new_dpi = 192;
    else
        new_dpi = 96;
    dpi_atom = XInternAtom(display, "DPI", False);
    property_value = new_dpi;
    XRRChangeOutputProperty(display, output, dpi_atom, XA_INTEGER, 32,
                            PropModeReplace,
                            (const unsigned char *)&property_value, 1);
    property_changed = True;
    XSync(display, False);
    if (!_XmXlibreDpiGetOutputDpi(display, output, &dpi) ||
        (long)(dpi + 0.5) != new_dpi)
        {
            status = fail("server did not expose the changed DPI property");
            goto cleanup;
        }
    if (!wait_for_callback(app) || callback_count != 1)
        {
            status = fail("property change was not coalesced into one callback");
            goto cleanup;
        }
    if (callback_old_output != output || callback_new_output != output ||
        fabs(callback_old_scale - old_dpi / _XM_XLIBRE_DPI_BASE) > 0.000001 ||
        fabs(callback_new_scale - new_dpi / _XM_XLIBRE_DPI_BASE) > 0.000001)
        {
            status = fail("callback reported the wrong output or scale transition");
            goto cleanup;
        }
    if (!_XmXlibreDpiGetWidgetScale(shell, &shell_scale) ||
        fabs(shell_scale - new_dpi / _XM_XLIBRE_DPI_BASE) > 0.000001)
        {
            status = fail("shell scale did not follow the property change");
            goto cleanup;
        }

cleanup:
    if (property_changed) {
        property_value = old_dpi;
        XRRChangeOutputProperty(display, output, dpi_atom, XA_INTEGER, 32,
                                PropModeReplace,
                                (const unsigned char *)&property_value, 1);
        XSync(display, False);
    }
    if (observer_installed)
        _XmXlibreDpiUninstallShell(shell);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    if (status != EXIT_SUCCESS)
        return status;
    puts("PASS XLibre live output-property transaction");
    return EXIT_SUCCESS;
}
