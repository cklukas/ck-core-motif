/* Verify the optional XLibre output-DPI contract and safe fallback. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "XlibreDpiI.h"

static int failed;

static void
expect_parse(const char *name, Atom type, int format, unsigned long items,
             unsigned long bytes_after, long value, int expected)
{
    double dpi = -1.0;
    int actual = _XmXlibreDpiParseProperty(type, format, items, bytes_after,
                                           value, &dpi);

    if (actual != expected || (expected && dpi != (double)value)) {
        fprintf(stderr, "%s: accepted=%d dpi=%g expected=%d\n", name, actual,
                dpi, expected);
        failed = 1;
    }
}

static void
check_property_parser(Display *display)
{
    Atom string_type = XInternAtom(display, "STRING", False);

    expect_parse("96 DPI", XA_INTEGER, 32, 1, 0, 96, 1);
    expect_parse("144 DPI", XA_INTEGER, 32, 1, 0, 144, 1);
    expect_parse("192 DPI", XA_INTEGER, 32, 1, 0, 192, 1);
    expect_parse("wrong type", string_type, 32, 1, 0, 96, 0);
    expect_parse("wrong format", XA_INTEGER, 16, 1, 0, 96, 0);
    expect_parse("multiple values", XA_INTEGER, 32, 2, 0, 96, 0);
    expect_parse("trailing bytes", XA_INTEGER, 32, 1, 4, 96, 0);
    expect_parse("zero", XA_INTEGER, 32, 1, 0, 0, 0);
    expect_parse("negative", XA_INTEGER, 32, 1, 0, -1, 0);
    expect_parse("oversized", XA_INTEGER, 32, 1, 0, 4097, 0);
}

static void
check_property_decode(Display *display)
{
    Atom string_type = XInternAtom(display, "STRING", False);
    const long valid_value = 144;
    double dpi = -1.0;

    if (!_XmXlibreDpiDecodeProperty(XA_INTEGER, 32, 1, 0,
                                    &valid_value, &dpi) || dpi != 144.0) {
        fprintf(stderr, "valid format-32 property did not decode\n");
        failed = 1;
    }
    /* Pass no buffer for malformed metadata.  The decoder must reject the
     * shape before dereferencing it; ASan/UBSan runs make this ordering
     * observable rather than merely checking the eventual Boolean result. */
    if (_XmXlibreDpiDecodeProperty(string_type, 8, 1, 0, NULL, &dpi) ||
        _XmXlibreDpiDecodeProperty(XA_INTEGER, 16, 1, 0, NULL, &dpi) ||
        _XmXlibreDpiDecodeProperty(XA_INTEGER, 32, 2, 0, NULL, &dpi) ||
        _XmXlibreDpiDecodeProperty(XA_INTEGER, 32, 1, 4, NULL, &dpi) ||
        _XmXlibreDpiDecodeProperty(XA_INTEGER, 32, 1, 0, NULL, &dpi)) {
        fprintf(stderr, "malformed property metadata was accepted\n");
        failed = 1;
    }
}

static void
check_server_contract(Display *display)
{
    const char *vendor = ServerVendor(display);
    int event_base;
    int error_base;
    int major;
    int minor;

    if (vendor != NULL && strstr(vendor, "XLibre") != NULL) {
        XRRScreenResources *resources;
        int connected = 0;
        int i;

        if (!XRRQueryExtension(display, &event_base, &error_base) ||
            !XRRQueryVersion(display, &major, &minor) || major < 1 ||
            (major == 1 && minor < 2)) {
            fprintf(stderr, "XLibre server lacks the required RandR version\n");
            failed = 1;
            return;
        }
        resources = XRRGetScreenResourcesCurrent(display,
                                                  DefaultRootWindow(display));
        if (resources == NULL) {
            fprintf(stderr, "could not query XLibre RandR outputs\n");
            failed = 1;
            return;
        }
        for (i = 0; i < resources->noutput; ++i) {
            double dpi;

            if (_XmXlibreDpiGetOutputDpi(display, resources->outputs[i], &dpi))
                ++connected;
            else {
                fprintf(stderr, "XLibre output %lu has no valid DPI property\n",
                        (unsigned long)resources->outputs[i]);
                failed = 1;
            }
        }
        XRRFreeScreenResources(resources);
        if (connected == 0) {
            fprintf(stderr, "XLibre server exposed no output DPI values\n");
            failed = 1;
        }
    } else if (_XmXlibreDpiServerSupported(display)) {
        fprintf(stderr, "non-XLibre vendor was accepted as XLibre\n");
        failed = 1;
    }
}

int
main(void)
{
    Display *display = XOpenDisplay(NULL);

    if (display == NULL) {
        fprintf(stderr, "cannot open DISPLAY\n");
        return EXIT_FAILURE;
    }
    check_property_parser(display);
    check_property_decode(display);
    check_server_contract(display);
    XCloseDisplay(display);
    if (failed)
        return EXIT_FAILURE;
    puts("PASS XLibre RandR DPI contract and non-XLibre fallback");
    return EXIT_SUCCESS;
}
