/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Large UTF-8 Motif clipboard round-trip regression. */
#include <Xm/Xm.h>
#include <Xm/CutPaste.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } \
    ++checks; \
} while (0)

static const unsigned char pattern[] =
    "ASCII ";
static const unsigned char utf8[] =
    "\xF0\x9F\x8C\x8D" /* globe */
    " \xE4\xB8\\xE7\x95\x8C" /* CJK */
    " \xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82" /* Cyrillic */
    " e\xCC\x81" /* combining acute */
    " \xD7\x90\xD7\x91\xD7\x92"; /* Hebrew */

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Display *display;
    Window window;
    const size_t repeats = 180000;
    const size_t prefix_length = sizeof(pattern) - 1;
    const size_t unit_length = sizeof(utf8) - 1;
    const size_t bytes = repeats * (prefix_length + unit_length);
    unsigned char *input, *output;
    size_t offset, split;
    unsigned long copied = 0, private_id = 0, item;
    int i;
    unsigned long checks = 0;

    shell = XtVaAppInitialize(&app, "UnicodeClipboardRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    XtRealizeWidget(shell);
    display = XtDisplay(shell);
    window = XtWindow(shell);
    input = malloc(bytes);
    output = malloc(bytes);
    CHECK(input && output);
    for (i = 0, offset = 0; (size_t)i < repeats; ++i) {
        memcpy(input + offset, pattern, prefix_length); offset += prefix_length;
        memcpy(input + offset, utf8, unit_length); offset += unit_length;
    }
    CHECK(offset == bytes);
    CHECK(XmClipboardRegisterFormat(display, "UTF8_STRING", 8) == XmClipboardSuccess);
    CHECK(XmClipboardStartCopy(display, window, NULL, CurrentTime,
                               shell, NULL, &item) == XmClipboardSuccess);
    split = 1048573; /* deliberately cuts through a multibyte sequence */
    CHECK(split < bytes);
    CHECK(XmClipboardCopy(display, window, item, "UTF8_STRING", input,
                          split, 77, NULL) == XmClipboardSuccess);
    CHECK(XmClipboardCopy(display, window, item, "UTF8_STRING", input + split,
                          bytes - split, 77, NULL) == XmClipboardSuccess);
    CHECK(XmClipboardEndCopy(display, window, item) == XmClipboardSuccess);
    CHECK(XmClipboardRetrieve(display, window, "UTF8_STRING", output, bytes,
                              &copied, &private_id) == XmClipboardSuccess);
    CHECK(copied == bytes && private_id == 77);
    CHECK(memcmp(input, output, bytes) == 0);
    free(output); free(input);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS large UTF-8 clipboard round trip: %lu checks\n", checks);
    return 0;
}
