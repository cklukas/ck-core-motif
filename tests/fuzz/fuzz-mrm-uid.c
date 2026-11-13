/*
 * Fuzz target: the UID file reader of the resource manager (lib/Mrm).
 *
 * The real lib/Mrm sources are compiled unchanged (run-fuzz.py builds them with
 * coverage instrumentation).  The input is the complete content of a UID file.
 * It is written to a file in TMPDIR, opened with MrmOpenHierarchyPerDisplay,
 * and every name that the file could define is fetched as a widget, literal,
 * colour, icon and bitmap, and applied with MrmFetchSetValues.  The names are
 * a fixed list plus every identifier-like string found in the input, so a
 * damaged copy of a valid file reaches its own records.
 *
 * Input encoding.  A UID file is a sequence of 4096-byte records, and a file
 * with an index and a few records is larger than the 4096 bytes that libFuzzer
 * is allowed to hand out.  The input is therefore the file with its zero bytes
 * run-length coded: a 0x00 is always followed by a count byte C and stands for
 * C + 1 zero bytes; every other byte stands for itself.  run-fuzz.py
 * --make-seeds writes the seeds in this form.
 *
 * Stubbed (mrm-stubs.c): Xlib, the Intrinsics and the widget library.  There is
 * no X server; widget creation returns plain core records.  A fault that is
 * reported inside a stand-in is a fault of the stand-in, not of the loader.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>

#include "mrm-stubs.h"

enum { MAX_NAMES = 40 };

static const char *fixed_names[] = {
    "module", "main_window", "main", "label", "button", "title", "width", "color",
    "icon", "bitmap", "menu", "text", "string", "font", "list",
};

static int identifier_char(unsigned char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    (void) argc; (void) argv;
    MrmInitialize();
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    static char path[1024];
    static char names[MAX_NAMES][48];
    size_t count = 0;

    if (!path[0]) {
        const char *tmp = getenv("TMPDIR");
        snprintf(path, sizeof path, "%s/mrm-uid-input-%d.uid", tmp ? tmp : ".", (int) getpid());
    }
    uint8_t *file = malloc(size * 128 + 1);
    size_t file_size = 0;
    if (!file) abort();
    for (size_t i = 0; i < size; i++) {
        if (data[i] == 0 && i + 1 < size) {
            memset(file + file_size, 0, (size_t) data[i + 1] + 1);
            file_size += (size_t) data[i + 1] + 1;
            i++;
        } else if (data[i] != 0) {
            file[file_size++] = data[i];
        }
    }
    data = file;
    size = file_size;
    FILE *f = fopen(path, "wb");
    if (!f) abort();
    fwrite(data, 1, size, f);
    fclose(f);

    for (size_t i = 0; i < sizeof fixed_names / sizeof *fixed_names; i++)
        snprintf(names[count++], sizeof names[0], "%s", fixed_names[i]);
    for (size_t i = 0; i < size && count < MAX_NAMES;) {
        size_t j = i;
        while (j < size && identifier_char(data[j])) j++;
        if (j - i >= 2 && j - i < sizeof names[0]) {
            memcpy(names[count], data + i, j - i);
            names[count][j - i] = 0;
            count++;
        }
        i = j + 1;
    }

    Display *display = fuzz_display();
    Screen *screen = fuzz_screen();
    Widget parent = fuzz_new_widget(NULL);
    MrmHierarchy hierarchy = NULL;
    String files[1] = { path };

    if (MrmOpenHierarchyPerDisplay(display, 1, files, NULL, &hierarchy) == MrmSUCCESS) {
        for (size_t i = 0; i < count; i++) {
            Widget w = NULL;
            MrmType class_return;
            XtPointer value = NULL;
            MrmCode type = 0;
            Pixel pixel;
            Pixmap pixmap;
            Dimension width, height;
            String name = names[i];

            if (MrmFetchWidget(hierarchy, name, parent, &w, &class_return) == MrmSUCCESS && w) {
                (void) w;
            }
            if (!w) w = fuzz_new_widget(parent);
            {
                /* the value of each argument names a resource of the file */
                static const char *tags[] = { "labelString", "width", "foreground", "labelPixmap",
                                              "fontList", "title", "accelerators" };
                for (size_t t = 0; t < sizeof tags / sizeof *tags; t++) {
                    Arg arg;
                    XtSetArg(arg, tags[t], (XtArgVal) name);
                    (void) MrmFetchSetValues(hierarchy, w, &arg, 1);
                }
            }
            if (MrmFetchLiteral(hierarchy, name, display, &value, &type) == MrmSUCCESS && value &&
                (type == MrmRtypeChar8 || type == MrmRtypeCString))
                XtFree(value);
            (void) MrmFetchColorLiteral(hierarchy, name, display, 1, &pixel);
            (void) MrmFetchIconLiteral(hierarchy, name, screen, display, 0, 1, &pixmap);
            (void) MrmFetchBitmapLiteral(hierarchy, name, screen, display, &pixmap, &width, &height);
        }
        Widget w = NULL;
        (void) MrmFetchInterfaceModule(hierarchy, "module", parent, &w);
        MrmCloseHierarchy(hierarchy);
    }
    fuzz_free_widgets();
    unlink(path);
    free(file);
    return 0;
}
