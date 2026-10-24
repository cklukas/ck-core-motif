/* Native regression for clipboard property storage, merge bounds and UTM teardown.
 * cc -std=c11 -Wall -Wextra -Werror check-transfer-storage.c \
 *   -I$prefix/include -L$prefix/lib -Wl,-rpath,$prefix/lib \
 *   -lXm -lXt -lX11 -ldl -o check-transfer-storage
 * Run each mode (clipboard, merge, lifetime) under a fresh Xvfb server.
 */
#include <Xm/Xm.h>
#include <Xm/CutPaste.h>
#include <Xm/TransferP.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <dlfcn.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", \
    __FILE__, __LINE__, #x); exit(1); } ++checks; } while (0)
static unsigned long checks;
static unsigned warnings;
static Widget watched;
static Atom disowned[32];
static unsigned disown_count;
static char *moving_value;
static size_t moving_bytes;

/* Realloc is allowed to move even small allocations. Force that legal
 * allocator outcome so source aliasing cannot hide behind an in-place grow. */
char *XtRealloc(char *pointer, Cardinal bytes)
{
    typedef char *(*ReallocProc)(char *, Cardinal);
    ReallocProc real = (ReallocProc)dlsym(RTLD_NEXT, "XtRealloc");
    if (!real) abort();
    if (pointer == moving_value && moving_value != NULL) {
        char *replacement = XtMalloc(bytes);
        CHECK(bytes >= moving_bytes);
        memcpy(replacement, pointer, moving_bytes);
        moving_value = NULL;
        XtFree(pointer);
        return replacement;
    }
    return real(pointer, bytes);
}

/* Observe the actual library call, then execute Xt's real implementation. */
void XtDisownSelection(Widget w, Atom selection, Time time)
{
    typedef void (*DisownProc)(Widget, Atom, Time);
    DisownProc real = (DisownProc)dlsym(RTLD_NEXT, "XtDisownSelection");
    if (!real) abort();
    if (w == watched) {
        CHECK(disown_count < XtNumber(disowned));
        disowned[disown_count++] = selection;
    }
    real(w, selection, time);
}

static void warning(String message)
{
    (void)message;
    ++warnings;
}

static void clipboard(Widget shell)
{
    Display *display = XtDisplay(shell);
    Window window = XtWindow(shell);
    const int formats[] = {8, 16, 32};
    /* Cross the reader's 1 MiB wire chunk, including tails and two appends. */
    const size_t counts[] = {3, 262145, 1048601};
    size_t f, c;
    for (f = 0; f < XtNumber(formats); ++f) {
        int bits = formats[f];
        size_t unit = bits == 32 ? sizeof(long) : (size_t)bits / 8;
        char name[64];
        snprintf(name, sizeof(name), "XM_TEST_STORAGE_%d", bits);
        CHECK(XmClipboardRegisterFormat(display, name, bits) == XmClipboardSuccess);
        for (c = 0; c < XtNumber(counts); ++c) {
            size_t n = counts[c], i, split = n / 2, bytes = n * unit;
            unsigned char *data = calloc(n, unit), *result = malloc(bytes);
            unsigned long length = 0, copied = 0;
            long item, id, private_id;
            fprintf(stderr, "clipboard format=%d items=%zu native_bytes=%zu\n", bits, n, bytes);
            CHECK(data && result);
            for (i = 0; i < n; ++i) {
                unsigned long value = (i * 2654435761UL + 0x80000001UL) & 0xffffffffUL;
                if (bits == 32) ((unsigned long *)data)[i] = value;
                else if (bits == 16) ((unsigned short *)data)[i] = (unsigned short)value;
                else data[i] = (unsigned char)value;
            }
            CHECK(XmClipboardStartCopy(display, window, NULL, CurrentTime,
                                       shell, NULL, &item) == XmClipboardSuccess);
            CHECK(XmClipboardCopy(display, window, item, name, data,
                                  split * unit, 1234, &id) == XmClipboardSuccess);
            CHECK(XmClipboardCopy(display, window, item, name, data + split * unit,
                                  bytes - split * unit, 1234, &id) == XmClipboardSuccess);
            CHECK(XmClipboardEndCopy(display, window, item) == XmClipboardSuccess);
            CHECK(XmClipboardInquireLength(display, window, name, &length) == XmClipboardSuccess);
            CHECK(length == bytes);
            CHECK(XmClipboardRetrieve(display, window, name, result, bytes,
                                      &copied, &private_id) == XmClipboardSuccess);
            CHECK(copied == bytes && private_id == 1234);
            if (bits == 32) {
                /* Xlib may sign-extend the wire value into each native long. */
                for (i = 0; i < n; ++i)
                    CHECK((((unsigned long *)result)[i] & 0xffffffffUL) ==
                          ((unsigned long *)data)[i]);
            } else CHECK(memcmp(data, result, bytes) == 0);
            free(result);
            free(data);
        }
    }
}

static void merge(XtAppContext app, int alias)
{
    const int formats[] = {8, 16, 32};
    unsigned f;
    XtAppSetWarningHandler(app, warning);
    for (f = 0; f < XtNumber(formats); ++f) {
        int bits = formats[f];
        size_t unit = bits == 32 ? sizeof(long) : (size_t)bits / 8;
        XmConvertCallbackStruct cs;
        unsigned char original[4 * sizeof(long)];
        unsigned i;
        memset(&cs, 0, sizeof(cs));
        for (i = 0; i < sizeof(original); ++i) original[i] = (unsigned char)(i + 1);
        cs.status = XmCONVERT_MERGE;
        cs.type = XA_INTEGER;
        cs.format = bits;
        cs.length = 2;
        cs.value = XtMalloc(2 * unit);
        memcpy(cs.value, original, 2 * unit);
        if (alias) {
            /* Aliasing append must survive allocation replacement. */
            moving_value = cs.value;
            moving_bytes = 2 * unit;
            XmeConvertMerge(cs.value, XA_INTEGER, bits, 2, &cs);
            moving_value = NULL;
            CHECK(cs.length == 4);
            CHECK(memcmp(cs.value, original, 2 * unit) == 0);
            CHECK(memcmp((char *)cs.value + 2 * unit, original, 2 * unit) == 0);
        }
        {
            XtPointer saved = cs.value;
            unsigned long saved_length = cs.length;
            unsigned old_warnings = warnings;
            XmeConvertMerge(original, XA_INTEGER, bits, ULONG_MAX, &cs);
            CHECK(cs.value == saved && cs.length == saved_length);
            CHECK(warnings == old_warnings + 1);
            XmeConvertMerge(NULL, XA_INTEGER, bits, 0, &cs);
            CHECK(cs.value == saved && cs.length == saved_length);
        }
        XtFree(cs.value);
    }
    CHECK(warnings == 3);
}

static void lifetime(Widget shell)
{
    Display *display = XtDisplay(shell);
    Atom named = XInternAtom(display, "XM_TEST_OWNED_SELECTION", False);
    Atom destination = XInternAtom(display, "_MOTIF_DESTINATION", False);
    Atom expected[] = {named, XA_PRIMARY, XA_SECONDARY, destination};
    unsigned round;
    Widget row = XtVaCreateManagedWidget("owners", xmRowColumnWidgetClass, shell, NULL);
    for (round = 0; round < 20; ++round) {
        unsigned i, j;
        Widget button = XtVaCreateManagedWidget("owner", xmPushButtonWidgetClass, row, NULL);
        Widget replacement = XtVaCreateManagedWidget("replacement", xmPushButtonWidgetClass, row, NULL);
        watched = button;
        disown_count = 0;
        for (i = 0; i < 3; ++i) {
            CHECK(XmeNamedSource(button, named, CurrentTime));
            CHECK(XmePrimarySource(button, CurrentTime));
            CHECK(XmeSecondarySource(button, CurrentTime));
            CHECK(XmeSecondarySink(button, CurrentTime));
        }
        /* Replace all cached contexts twice before close. Their display-close
         * callbacks must be removed when each old context is freed early. */
        CHECK(XmeNamedSource(replacement, named, CurrentTime));
        CHECK(XmePrimarySource(replacement, CurrentTime));
        CHECK(XmeSecondarySource(replacement, CurrentTime));
        CHECK(XmeSecondarySink(replacement, CurrentTime));
        CHECK(XmeNamedSource(button, named, CurrentTime));
        CHECK(XmePrimarySource(button, CurrentTime));
        CHECK(XmeSecondarySource(button, CurrentTime));
        CHECK(XmeSecondarySink(button, CurrentTime));
        XtDestroyWidget(replacement);
        for (i = 0; i < XtNumber(expected); ++i)
            CHECK(XGetSelectionOwner(display, expected[i]) == XtWindow(button));
        XtDestroyWidget(button);
        CHECK(disown_count == XtNumber(expected));
        for (i = 0; i < XtNumber(expected); ++i) {
            unsigned count = 0;
            for (j = 0; j < disown_count; ++j)
                if (disowned[j] == expected[i]) ++count;
            CHECK(count == 1);
            CHECK(XGetSelectionOwner(display, expected[i]) == None);
        }
        watched = NULL;
    }
    XtDestroyWidget(row);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Display *display;
    CHECK(argc == 2);
    shell = XtVaAppInitialize(&app, "TransferStorage", NULL, 0, &argc, argv,
                              NULL, XtNwidth, 200, XtNheight, 100, NULL);
    XtRealizeWidget(shell);
    display = XtDisplay(shell);
    if (strcmp(argv[1], "clipboard") == 0) clipboard(shell);
    else if (strcmp(argv[1], "merge") == 0) merge(app, 1);
    else if (strcmp(argv[1], "bounds") == 0) merge(app, 0);
    else if (strcmp(argv[1], "lifetime") == 0) lifetime(shell);
    else return 2;
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS %s: %lu checks\n", argv[1], checks);
    return 0;
}
