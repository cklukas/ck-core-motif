/*
 * Fuzz target: the compound string external format (ASN.1 byte stream) of
 * lib/Xm/XmString.c.
 *
 * XmString.c, XmRenderT.c, XmTabList.c, Direction.c, Xmos.c, ResEncod.c and
 * ResInd.c are compiled unchanged (run-fuzz.py builds them with coverage
 * instrumentation); xm-stubs.c stands in for the display and the widgets.
 *
 * The input is a byte stream as XmCvtByteStreamToXmString receives it, for
 * example the content of a clipboard or drag-and-drop property:
 *
 *   df 80 06 <length> <component>*      component: tag, length, value
 *
 * The function has no length parameter: it trusts the stream's own header, as
 * XmStringByteStreamLength does.  A caller that holds N bytes can therefore
 * only hand it a stream whose header length fits in N bytes.  The harness is
 * that careful caller: a stream whose declared length exceeds the bytes held is
 * not passed on.  The stream is passed in a heap block of exactly the held
 * size, so everything the components claim beyond that is an address error.
 *
 * Every parsed string is walked (triples and segments), copied, concatenated,
 * compared, converted back to a byte stream and parsed again.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

/* The header length as XmStringByteStreamLength reads it, bounds-checked here. */
static int declared_size(const uint8_t *p, size_t size, size_t *total)
{
    if (size < 4 || p[0] != 0xdf || p[1] != 0x80 || p[2] != 0x06) return 0;
    size_t len, off;
    if (!(p[3] & 0x80)) { len = p[3]; off = 4; }
    else {
        if (size < 6) return 0;                  /* a marker byte with the high bit, then two length bytes */
        len = ((size_t) p[4] << 8) | p[5];
        off = 6;
    }
    *total = off + len;
    return *total <= size;
}

/* The components, as XmCvtByteStreamToXmString walks them, must tile the stream exactly.
 * FUZZ_WELLFORMED=1 only passes such streams: a caller that validates the stream first. */
static int components_fit(const uint8_t *p, size_t total)
{
    size_t pos = (p[3] & 0x80) ? 6 : 4;
    while (pos < total) {
        size_t header, len;
        if (total - pos < 2) return 0;
        if (p[pos + 1] & 0x80) {
            if (total - pos < 4) return 0;
            len = ((size_t) p[pos + 2] << 8) | p[pos + 3];
            header = 4;
        } else {
            len = p[pos + 1];
            header = 2;
        }
        if (len > total - pos - header) return 0;
        pos += header + len;
    }
    return 1;
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    (void) argc; (void) argv;
    setlocale(LC_ALL, "C");
    return 0;
}

static void walk(XmString s)
{
    XmStringContext ctx;
    XtPointer value;
    unsigned int len;
    XmStringComponentType t;
    char *text, *tag;
    XmStringDirection dir;
    Boolean sep;

    if (XmStringInitContext(&ctx, s)) {
        int guard = 0;
        while ((t = XmStringGetNextTriple(ctx, &len, &value)) != XmSTRING_COMPONENT_END && guard++ < 100000) {
            volatile unsigned char sink = 0;
            for (unsigned i = 0; value && i < len; i++) sink ^= ((unsigned char *) value)[i];
            if (value) XtFree(value);
        }
        XmStringFreeContext(ctx);
    }
    /* FUZZ_SKIP_SEGMENTS=1 leaves out XmStringGetNextSegment, to look past a finding in it */
    if (!getenv("FUZZ_SKIP_SEGMENTS") && XmStringInitContext(&ctx, s)) {
        int guard = 0;
        while (XmStringGetNextSegment(ctx, &text, &tag, &dir, &sep) && guard++ < 100000) {
            if (text) { (void) strlen(text); XtFree(text); }
            if (tag) XtFree(tag);
        }
        XmStringFreeContext(ctx);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    size_t total;
    if (!declared_size(data, size, &total)) return 0;

    if (getenv("FUZZ_WELLFORMED") && !components_fit(data, total)) return 0;

    uint8_t *copy = malloc(total);
    if (!copy) abort();
    memcpy(copy, data, total);

    (void) XmStringByteStreamLength(copy);
    XmString s = XmCvtByteStreamToXmString(copy);
    if (s) {
        walk(s);
        (void) XmStringEmpty(s);
        (void) XmStringLineCount(s);
        (void) XmStringByteCompare(s, s);
        XmString c = XmStringCopy(s);
        if (c) {
            XmString joined = XmStringConcat(s, c);
            if (joined) { walk(joined); XmStringFree(joined); }
            XmStringFree(c);
        }
        unsigned char *out = NULL;
        unsigned n = XmCvtXmStringToByteStream(s, &out);
        if (out) {
            if (XmStringByteStreamLength(out) != n) { fprintf(stderr, "stream length mismatch\n"); abort(); }
            XmString again = XmCvtByteStreamToXmString(out);
            if (again) XmStringFree(again);
            XtFree((char *) out);
        }
        XmStringFree(s);
    }
    free(copy);
    return 0;
}
