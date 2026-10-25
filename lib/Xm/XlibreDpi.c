/*
 * Optional XLibre RandR output-DPI contract support.
 *
 * The parser and selector are usable without a live server.  When the
 * opt-in XLibre build is enabled, realized shells can additionally register
 * a UI-thread RandR observer.  The observer coalesces notifications into one
 * Xt work procedure and leaves explicit widget resources unchanged; callers
 * use the documented change hook to update derived metrics.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <X11/Xatom.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "XmI.h"
#include "XlibreDpiI.h"

#define XLIBRE_DPI_PROPERTY "DPI"

typedef struct _XmXlibreDpiShellState _XmXlibreDpiShellState;
struct _XmXlibreDpiShellState {
    Widget shell;
    Display *display;
    Window root;
    int event_base;
    XtInputId input_id;
    XtWorkProcId work_id;
    Boolean work_pending;
    RROutput output;
    double scale;
    _XmXlibreDpiChangeProc change_proc;
    XtPointer closure;
    unsigned int references;
    Boolean retired;
    _XmXlibreDpiShellState *next;
};

static _XmXlibreDpiShellState *shell_states;

static void
_XmXlibreDpiUninstallState(_XmXlibreDpiShellState *state,
                           Boolean remove_destroy_callback);

static void
_XmXlibreDpiRelease(_XmXlibreDpiShellState *state)
{
    if (state == NULL || state->references == 0)
        return;
    --state->references;
    if (state->references == 0 && state->retired)
        free(state);
}

static void
_XmXlibreDpiRetain(_XmXlibreDpiShellState *state)
{
    if (state != NULL)
        ++state->references;
}

static Boolean
_XmXlibreDpiIsNotifyEvent(const _XmXlibreDpiShellState *state,
                           const XEvent *event)
{
    return state != NULL && event != NULL &&
           (event->type == state->event_base + RRScreenChangeNotify ||
            event->type == state->event_base + RRNotify);
}

static Bool
_XmXlibreDpiEventPredicate(Display *display,
                           XEvent *event,
                           XPointer closure)
{
    _XmXlibreDpiShellState *state =
        (_XmXlibreDpiShellState *)closure;
    (void)display;
    return _XmXlibreDpiIsNotifyEvent(state, event);
}

static void
_XmXlibreDpiSchedule(_XmXlibreDpiShellState *state);

static void
_XmXlibreDpiInput(XtPointer closure, int *source, XtInputId *input_id)
{
    _XmXlibreDpiShellState *state = (_XmXlibreDpiShellState *)closure;
    XEvent event;
    (void)source;
    (void)input_id;

    if (state == NULL || state->display == NULL)
        return;

    /* Consume only RandR notifications; Xt remains responsible for all
     * ordinary X events in the display queue. */
    while (XCheckIfEvent(state->display, &event, _XmXlibreDpiEventPredicate,
                         (XPointer)state)) {
        _XmXlibreDpiShellState *current;
        for (current = shell_states; current != NULL; current = current->next)
            if (current->display == state->display)
                _XmXlibreDpiSchedule(current);
    }
}

static Boolean
_XmXlibreDpiClientArea(_XmXlibreDpiShellState *state,
                       _XmXlibreDpiRect *area_return)
{
    XWindowAttributes attributes;
    Window child;
    int x;
    int y;

    if (state == NULL || area_return == NULL || !XtIsRealized(state->shell))
        return False;
    if (!XGetWindowAttributes(state->display, XtWindow(state->shell),
                              &attributes) ||
        !XTranslateCoordinates(state->display, XtWindow(state->shell),
                               state->root, 0, 0, &x, &y, &child))
        return False;
    area_return->x = x;
    area_return->y = y;
    area_return->width = attributes.width > 0 ? (unsigned int)attributes.width : 0;
    area_return->height = attributes.height > 0 ? (unsigned int)attributes.height : 0;
    return True;
}

static Boolean
_XmXlibreDpiResolve(_XmXlibreDpiShellState *state,
                    RROutput *output_return,
                    double *scale_return)
{
    XRRScreenResources *resources;
    _XmXlibreDpiOutput *outputs;
    _XmXlibreDpiRect area;
    size_t output_count = 0;
    size_t i;
    RROutput selected;
    double global_scale = 1.0;
    Boolean have_global;
    Boolean have_output = False;

    if (state == NULL || output_return == NULL || scale_return == NULL ||
        !_XmXlibreDpiClientArea(state, &area))
        return False;
    resources = XRRGetScreenResourcesCurrent(state->display, state->root);
    if (resources == NULL)
        return False;
    outputs = ( _XmXlibreDpiOutput *)calloc(resources->noutput,
                                             sizeof(*outputs));
    if (outputs == NULL) {
        XRRFreeScreenResources(resources);
        return False;
    }
    for (i = 0; i < (size_t)resources->noutput; ++i) {
        XRROutputInfo *output_info = XRRGetOutputInfo(
            state->display, resources, resources->outputs[i]);
        XRRCrtcInfo *crtc_info;
        double dpi;
        if (output_info == NULL)
            continue;
        if (output_info->connection != RR_Connected ||
            output_info->crtc == None) {
            XRRFreeOutputInfo(output_info);
            continue;
        }
        crtc_info = XRRGetCrtcInfo(state->display, resources,
                                   output_info->crtc);
        if (crtc_info != NULL && crtc_info->width != 0 &&
            crtc_info->height != 0) {
            outputs[output_count].output = resources->outputs[i];
            outputs[output_count].geometry.x = crtc_info->x;
            outputs[output_count].geometry.y = crtc_info->y;
            outputs[output_count].geometry.width = crtc_info->width;
            outputs[output_count].geometry.height = crtc_info->height;
            if (_XmXlibreDpiGetOutputDpi(state->display,
                                         resources->outputs[i], &dpi))
                have_output = True;
            ++output_count;
        }
        if (crtc_info != NULL)
            XRRFreeCrtcInfo(crtc_info);
        XRRFreeOutputInfo(output_info);
    }
    selected = _XmXlibreDpiSelectOutput(outputs, output_count, &area);
    have_global = _XmGetGlobalDpiScale(state->display, &global_scale);
    *output_return = selected;
    *scale_return = have_global ? global_scale : 1.0;
    if (selected != None) {
        for (i = 0; i < output_count; ++i) {
            double dpi;
            if (outputs[i].output == selected &&
                _XmXlibreDpiGetOutputDpi(state->display, selected, &dpi)) {
                *scale_return = dpi / _XM_XLIBRE_DPI_BASE;
                break;
            }
        }
    } else if (have_output) {
        /* A transient RandR gap is not permission to guess a new scale. */
        *scale_return = have_global ? global_scale : 1.0;
    }
    free(outputs);
    XRRFreeScreenResources(resources);
    return True;
}

static Boolean
_XmXlibreDpiWork(XtPointer closure)
{
    _XmXlibreDpiShellState *state = (_XmXlibreDpiShellState *)closure;
    RROutput output;
    double scale;
    double old_scale;
    RROutput old_output;

    if (state == NULL)
        return True;
    /* The destroy callback may retire the registry reference while a resize
     * callback is still unwinding. Keep this work-procedure reference until
     * every reentrant callback has returned. */
    _XmXlibreDpiRetain(state);
    state->work_pending = False;
    state->work_id = (XtWorkProcId)0;
    if (state->retired || !_XmXlibreDpiResolve(state, &output, &scale)) {
        _XmXlibreDpiRelease(state);
        return True;
    }
    old_scale = state->scale;
    old_output = state->output;
    state->scale = scale;
    state->output = output;
    if (old_output == output && fabs(old_scale - scale) < 0.000001) {
        _XmXlibreDpiRelease(state);
        return True;
    }

    /* Use the same UI-thread transaction as deterministic developer
     * overrides.  Registered widgets update only toolkit-owned derived
     * defaults; explicit application resources remain untouched. */
    if (!_XmApplyScaleChangeForWidget(state->shell, old_scale, scale)) {
        state->scale = old_scale;
        state->output = old_output;
        _XmXlibreDpiRelease(state);
        return True;
    }
    if (!state->retired && state->change_proc != NULL)
        state->change_proc(state->shell, old_scale, scale, old_output, output,
                           state->closure);
    _XmXlibreDpiRelease(state);
    return True;
}

static void
_XmXlibreDpiSchedule(_XmXlibreDpiShellState *state)
{
    if (state != NULL && !state->work_pending) {
        state->work_pending = True;
        state->work_id = XtAppAddWorkProc(
            XtWidgetToApplicationContext(state->shell),
            _XmXlibreDpiWork, (XtPointer)state);
    }
}

static void
_XmXlibreDpiDestroy(Widget shell, XtPointer closure, XtPointer call_data)
{
    _XmXlibreDpiShellState *state = (_XmXlibreDpiShellState *)closure;
    (void)shell;
    (void)call_data;
    if (state != NULL)
        _XmXlibreDpiUninstallState(state, False);
}

static _XmXlibreDpiShellState *
_XmXlibreDpiFindShell(Widget widget)
{
    Widget current = widget;
    _XmXlibreDpiShellState *state;

    while (current != NULL && !XtIsShell(current))
        current = XtParent(current);
    for (state = shell_states; state != NULL; state = state->next)
        if (state->shell == current)
            return state;
    return NULL;
}

Boolean
_XmXlibreDpiInstallShell(Widget shell,
                         _XmXlibreDpiChangeProc change_proc,
                         XtPointer closure)
{
    _XmXlibreDpiShellState *state;
    int event_base;
    int error_base;
    RROutput output = None;
    double scale = 1.0;
    Window root;
    Display *display;
    XtAppContext app_context;

    if (shell == NULL || !XtIsShell(shell) || !XtIsRealized(shell))
        return False;
    display = XtDisplay(shell);
    if (!_XmXlibreDpiServerSupported(display) ||
        !XRRQueryExtension(display, &event_base, &error_base))
        return False;
    state = _XmXlibreDpiFindShell(shell);
    if (state != NULL) {
        state->change_proc = change_proc;
        state->closure = closure;
        return True;
    }
    state = (_XmXlibreDpiShellState *)calloc(1, sizeof(*state));
    if (state == NULL)
        return False;
    state->shell = shell;
    state->display = display;
    state->root = RootWindowOfScreen(XtScreen(shell));
    if (!XRRQueryExtension(display, &state->event_base, &error_base)) {
        free(state);
        return False;
    }
    state->output = None;
    state->scale = _XmGetGlobalDpiScale(display, &scale) ? scale : 1.0;
    if (_XmXlibreDpiResolve(state, &output, &scale)) {
        state->output = output;
        state->scale = scale;
    }
    state->change_proc = change_proc;
    state->closure = closure;
    state->references = 1; /* registry ownership */
    root = state->root;
    XRRSelectInput(display, root, RRScreenChangeNotifyMask |
                   RROutputChangeNotifyMask | RROutputPropertyNotifyMask);
    app_context = XtWidgetToApplicationContext(shell);
    state->input_id = XtAppAddInput(app_context, ConnectionNumber(display),
                                    (XtPointer)XtInputReadMask,
                                    _XmXlibreDpiInput, (XtPointer)state);
    XtAddCallback(shell, XtNdestroyCallback, _XmXlibreDpiDestroy,
                  (XtPointer)state);
    state->next = shell_states;
    shell_states = state;
    return True;
}

void
_XmXlibreDpiUninstallShell(Widget shell)
{
    _XmXlibreDpiShellState *state;

    state = _XmXlibreDpiFindShell(shell);
    if (state == NULL)
        return;
    _XmXlibreDpiUninstallState(state, True);
}

static void
_XmXlibreDpiUninstallState(_XmXlibreDpiShellState *state,
                           Boolean remove_destroy_callback)
{
    _XmXlibreDpiShellState **link = &shell_states;

    while (*link != NULL && *link != state)
        link = &(*link)->next;
    if (*link == NULL)
        return;
    *link = state->next;
    state->retired = True;
    if (remove_destroy_callback)
        XtRemoveCallback(state->shell, XtNdestroyCallback,
                         _XmXlibreDpiDestroy, (XtPointer)state);
    if (state->input_id != (XtInputId)0)
        XtRemoveInput(state->input_id);
    if (state->work_pending && state->work_id != (XtWorkProcId)0)
        XtRemoveWorkProc(state->work_id);
    _XmXlibreDpiRelease(state); /* drop registry ownership */
}

Boolean
_XmXlibreDpiGetWidgetScale(Widget widget, double *scale_return)
{
    _XmXlibreDpiShellState *state;
    if (scale_return == NULL)
        return False;
    state = _XmXlibreDpiFindShell(widget);
    if (state == NULL)
        return False;
    *scale_return = state->scale;
    return True;
}

Boolean
_XmXlibreDpiParseProperty(Atom actual_type,
                           int actual_format,
                           unsigned long item_count,
                           unsigned long bytes_after,
                           long property_value,
                           double *dpi_return)
{
    if (dpi_return == NULL || actual_type != XA_INTEGER || actual_format != 32 ||
        item_count != 1 || bytes_after != 0 ||
        property_value < _XM_XLIBRE_DPI_MIN ||
        property_value > _XM_XLIBRE_DPI_MAX)
        return False;

    *dpi_return = (double)property_value;
    return True;
}

Boolean
_XmXlibreDpiServerSupported(Display *display)
{
    int event_base;
    int error_base;
    int major;
    int minor;
    const char *vendor;

    if (display == NULL || !XRRQueryExtension(display, &event_base, &error_base) ||
        !XRRQueryVersion(display, &major, &minor))
        return False;
    if (major < 1 || (major == 1 && minor < 2))
        return False;

    vendor = ServerVendor(display);
    return vendor != NULL && strstr(vendor, "XLibre") != NULL;
}

Boolean
_XmXlibreDpiGetOutputDpi(Display *display,
                         RROutput output,
                         double *dpi_return)
{
    Atom property;
    Atom actual_type = None;
    int actual_format = 0;
    unsigned long item_count = 0;
    unsigned long bytes_after = 0;
    unsigned char *data = NULL;
    int status;
    Boolean valid;

    if (dpi_return == NULL || !_XmXlibreDpiServerSupported(display))
        return False;

    property = XInternAtom(display, XLIBRE_DPI_PROPERTY, True);
    if (property == None)
        return False;

    status = XRRGetOutputProperty(display, output, property, 0, 1, False,
                                  False, AnyPropertyType, &actual_type,
                                  &actual_format, &item_count, &bytes_after,
                                  &data);
    if (status != Success || data == NULL)
        return False;

    /* Xlib presents format-32 property data as a long-sized value. */
    valid = _XmXlibreDpiParseProperty(actual_type, actual_format,
                                       item_count, bytes_after,
                                       ((long *)data)[0], dpi_return);
    XFree(data);
    return valid;
}

static long long
_XmXlibreDpiDistance(long long center,
                     long long begin,
                     unsigned int size)
{
    long long end;
    long long distance;

    if (size == 0)
        return -1;
    end = begin + (long long)size - 1;
    if (center < begin)
        distance = begin - center;
    else if (center > end)
        distance = center - end;
    else
        distance = 0;
    return distance;
}

RROutput
_XmXlibreDpiSelectOutput(const _XmXlibreDpiOutput *outputs,
                         size_t output_count,
                         const _XmXlibreDpiRect *client_area)
{
    long long center_x;
    long long center_y;
    RROutput selected = None;
    long long selected_distance = 0;
    size_t i;

    if (outputs == NULL || output_count == 0 || client_area == NULL ||
        client_area->width == 0 || client_area->height == 0)
        return None;

    center_x = (long long)client_area->x + client_area->width / 2;
    center_y = (long long)client_area->y + client_area->height / 2;
    for (i = 0; i < output_count; ++i) {
        const _XmXlibreDpiRect *geometry = &outputs[i].geometry;
        long long left = geometry->x;
        long long top = geometry->y;
        long long right;
        long long bottom;
        long long dx;
        long long dy;
        long long distance;

        if (geometry->width == 0 || geometry->height == 0 ||
            outputs[i].output == None)
            continue;
        right = left + geometry->width;
        bottom = top + geometry->height;
        dx = _XmXlibreDpiDistance(center_x, left, geometry->width);
        dy = _XmXlibreDpiDistance(center_y, top, geometry->height);
        distance = dx + dy;
        if (center_x >= left && center_x < right && center_y >= top &&
            center_y < bottom)
            distance = 0;

        if (selected == None || distance < selected_distance ||
            (distance == selected_distance && outputs[i].output < selected)) {
            selected = outputs[i].output;
            selected_distance = distance;
        }
    }
    return selected;
}
