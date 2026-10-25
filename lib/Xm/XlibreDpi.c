/*
 * Optional XLibre RandR output-DPI contract support.
 *
 * This module only validates and reads the server hint.  It deliberately does
 * not mutate widget geometry; live shell selection and reflow are a later
 * package with a separate ownership and event-queue contract.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <X11/Xatom.h>

#include "XlibreDpiI.h"

#define XLIBRE_DPI_PROPERTY "DPI"

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
