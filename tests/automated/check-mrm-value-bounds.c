/*
 * UID file reader: compound string literals whose stream is shorter than the
 * length that its own header declares.  Run by run-mrm-value-bounds.py with
 * the production lib/Mrm sources compiled under AddressSanitizer and
 * UndefinedBehaviorSanitizer.
 *
 *   check-mrm-value-bounds CASE TMPDIR
 *
 * The UID files are written with the resource manager's own writer and read
 * back with MrmFetchSetValues and MrmFetchLiteral.  XmCvtByteStreamToXmString
 * has no length parameter; its stand-in in tests/fuzz/mrm-stubs.c reads as many
 * bytes as the header declares, as the real converter does.
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>
#include <Mrm/Mrm.h>

#include "mrm-stubs.h"

/* The toolkit headers redefine assert(), so the checks use their own macro. */
#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #condition); \
            abort(); \
        } \
    } while (0)

static char *get_memory(unsigned n) { return XtMalloc(n); }
static void free_memory(char *p) { XtFree(p); }

static URMResourceContextPtr new_context(size_t size)
{
    URMResourceContextPtr context = NULL;

    CHECK(UrmGetResourceContext(get_memory, free_memory, (int) size, &context) == MrmSUCCESS);
    return context;
}

/* A context holding SIZE bytes of DATA, as the writer fills one. */
static URMResourceContextPtr filled_context(MrmType type, const void *data, size_t size)
{
    URMResourceContextPtr context = new_context(size);

    memcpy(UrmRCBuffer(context), data, size);
    UrmRCSetSize(context, (MrmSize) size);
    UrmRCSetType(context, type);
    UrmRCSetGroup(context, URMgLiteral);
    UrmRCSetAccess(context, URMaPublic);
    return context;
}

static char *uid_path(const char *dir, const char *name)
{
    static char path[1024];

    snprintf(path, sizeof path, "%s/check-mrm-%s-%d.uid", dir, name, (int) getpid());
    return path;
}

static IDBFile open_for_writing(const char *path)
{
    IDBFile file = NULL;

    CHECK(UrmIdbOpenFileWrite(path, NULL, "check", "1.0", "module", "1.0", &file, NULL) == MrmSUCCESS);
    return file;
}

static void put_literal(IDBFile file, const char *name, MrmType type, const void *data, size_t size)
{
    URMResourceContextPtr context = filled_context(type, data, size);

    CHECK(UrmPutIndexedLiteral(file, (String) name, context) == MrmSUCCESS);
    UrmFreeResourceContext(context);
}

/* A stream of PAYLOAD bytes behind the ASN.1 header; LONG selects the three byte length. */
static size_t make_stream(unsigned char *out, size_t payload, int long_form)
{
    size_t n = 0;

    out[n++] = 0xdf; out[n++] = 0x80; out[n++] = 0x06;
    if (long_form) {
        out[n++] = 0x82; out[n++] = (unsigned char) (payload >> 8); out[n++] = (unsigned char) payload;
    } else
        out[n++] = (unsigned char) payload;
    memset(out + n, 0x41, payload);
    return n + payload;
}

/* Writes a file with the single literal "literal" of STREAM (SIZE bytes, exactly) and
 * fetches it as a widget value. *APPLIED is the number of arguments that reached the widget. */
static Cardinal fetch_value_as(const char *dir, const char *name, MrmType type, const void *stream,
                               size_t size, Cardinal *applied)
{
    char *path = uid_path(dir, name);
    IDBFile file = open_for_writing(path);
    MrmHierarchy hierarchy = NULL;
    String files[1] = { path };
    Widget parent = fuzz_new_widget(NULL);
    Widget widget = fuzz_new_widget(parent);
    Arg arg;
    Cardinal status;

    put_literal(file, "literal", type, stream, size);
    CHECK(UrmIdbCloseFile(file, TRUE) == MrmSUCCESS);
    CHECK(MrmOpenHierarchyPerDisplay(fuzz_display(), 1, files, NULL, &hierarchy) == MrmSUCCESS);
    XtSetArg(arg, "labelString", (XtArgVal) "literal");
    fuzz_set_values_count = 0;
    status = MrmFetchSetValues(hierarchy, widget, &arg, 1);
    *applied = fuzz_set_values_count;
    MrmCloseHierarchy(hierarchy);
    fuzz_free_widgets();
    unlink(path);
    return status;
}

/* The same literal through MrmFetchLiteral. */
static Cardinal fetch_literal_as(const char *dir, const char *name, MrmType type, const void *stream,
                                 size_t size)
{
    char *path = uid_path(dir, name);
    IDBFile file = open_for_writing(path);
    MrmHierarchy hierarchy = NULL;
    String files[1] = { path };
    XtPointer value = NULL;
    MrmCode fetched = 0;
    Cardinal status;

    put_literal(file, "literal", type, stream, size);
    CHECK(UrmIdbCloseFile(file, TRUE) == MrmSUCCESS);
    CHECK(MrmOpenHierarchyPerDisplay(fuzz_display(), 1, files, NULL, &hierarchy) == MrmSUCCESS);
    status = MrmFetchLiteral(hierarchy, "literal", fuzz_display(), &value, &fetched);
    MrmCloseHierarchy(hierarchy);
    unlink(path);
    return status;
}

static Cardinal fetch_cstring(const char *dir, const char *name, const void *stream, size_t size,
                              Cardinal *applied)
{
    return fetch_value_as(dir, name, MrmRtypeCString, stream, size, applied);
}

static Cardinal fetch_literal_cstring(const char *dir, const char *name, const void *stream, size_t size)
{
    return fetch_literal_as(dir, name, MrmRtypeCString, stream, size);
}

/* The stream declares more bytes than the literal holds. */
static void short_stream(const char *dir)
{
    unsigned char stream[300];
    size_t size = make_stream(stream, 40, 0);
    Cardinal applied;

    CHECK(fetch_cstring(dir, "short", stream, size - 1, &applied) != MrmSUCCESS);
    CHECK(applied == 0);
    CHECK(fetch_cstring(dir, "short-by-half", stream, size / 2, &applied) != MrmSUCCESS);
    CHECK(applied == 0);
    size = make_stream(stream, 260, 1);
    CHECK(fetch_cstring(dir, "short-long", stream, size - 1, &applied) != MrmSUCCESS);
    CHECK(applied == 0);
}

/* The literal ends inside the header, or in the three byte length. */
static void truncated_header(const char *dir)
{
    static const unsigned char stream[] = { 0xdf, 0x80, 0x06, 0x82, 0x01, 0x00 };
    Cardinal applied;

    for (size_t size = 1; size < sizeof stream; size++) {
        CHECK(fetch_cstring(dir, "truncated", stream, size, &applied) != MrmSUCCESS);
        CHECK(applied == 0);
    }
}

static void short_stream_fetch_literal(const char *dir)
{
    unsigned char stream[300];
    size_t size = make_stream(stream, 40, 0);

    CHECK(fetch_literal_cstring(dir, "fetch-short", stream, size - 1) != MrmSUCCESS);
    CHECK(fetch_literal_cstring(dir, "fetch-header", stream, 3) != MrmSUCCESS);
}

/* A stream that fills its literal exactly converts, in both length forms. */
static void valid_stream(const char *dir)
{
    unsigned char stream[300];
    Cardinal applied;
    size_t size = make_stream(stream, 40, 0);

    CHECK(fetch_cstring(dir, "valid", stream, size, &applied) == MrmSUCCESS);
    CHECK(applied == 1);
    size = make_stream(stream, 260, 1);
    CHECK(fetch_cstring(dir, "valid-long", stream, size, &applied) == MrmSUCCESS);
    CHECK(applied == 1);
    size = make_stream(stream, 40, 0);
    CHECK(fetch_literal_cstring(dir, "valid-fetch", stream, size) == MrmSUCCESS);
}


/* ---- colours ---- */

/* A colour descriptor of TYPE with PAYLOAD bytes behind its header, filled with FILL. */
static size_t make_colour(unsigned char *out, int type, size_t payload, unsigned char fill)
{
    RGMColorDesc desc;
    size_t header = offsetof(RGMColorDesc, desc);

    memset(&desc, 0, sizeof desc);
    desc.desc_type = (MrmSCode) type;
    memcpy(out, &desc, header);
    memset(out + header, fill, payload);
    return header + payload;
}

/* A colour through MrmFetchColorLiteral. */
static Cardinal fetch_colour(const char *dir, const char *name, const void *data, size_t size)
{
    char *path = uid_path(dir, name);
    IDBFile file = open_for_writing(path);
    MrmHierarchy hierarchy = NULL;
    String files[1] = { path };
    Pixel pixel = 0;
    Cardinal status;

    put_literal(file, "literal", MrmRtypeColor, data, size);
    CHECK(UrmIdbCloseFile(file, TRUE) == MrmSUCCESS);
    CHECK(MrmOpenHierarchyPerDisplay(fuzz_display(), 1, files, NULL, &hierarchy) == MrmSUCCESS);
    status = MrmFetchColorLiteral(hierarchy, "literal", fuzz_display(), 0, &pixel);
    MrmCloseHierarchy(hierarchy);
    unlink(path);
    return status;
}

/* A colour name without its terminating NUL character, or a descriptor without its name. */
static void colour_name_not_terminated(const char *dir)
{
    unsigned char colour[64];
    Cardinal applied;
    size_t size = make_colour(colour, URMColorDescTypeName, 8, 'a');

    CHECK(fetch_colour(dir, "colour-name", colour, size) != MrmSUCCESS);
    CHECK(fetch_value_as(dir, "colour-name-set", MrmRtypeColor, colour, size, &applied) != MrmSUCCESS);
    CHECK(applied == 0);
    size = make_colour(colour, URMColorDescTypeName, 0, 0);
    CHECK(fetch_colour(dir, "colour-empty", colour, size) != MrmSUCCESS);
}

/* An RGB colour that is shorter than its three components. */
static void colour_rgb_short(const char *dir)
{
    unsigned char colour[64];
    Cardinal applied;
    size_t size = make_colour(colour, URMColorDescTypeRGB, sizeof(RGBColor) - 1, 0x10);

    CHECK(fetch_colour(dir, "colour-rgb", colour, size) != MrmSUCCESS);
    CHECK(fetch_value_as(dir, "colour-rgb-set", MrmRtypeColor, colour, size, &applied) != MrmSUCCESS);
    CHECK(applied == 0);
}

static void valid_colour(const char *dir)
{
    unsigned char colour[64];
    Cardinal applied;
    size_t size = make_colour(colour, URMColorDescTypeName, 4, 'r');

    colour[size - 1] = '\0';
    CHECK(fetch_colour(dir, "valid-name", colour, size) == MrmSUCCESS);
    CHECK(fetch_value_as(dir, "valid-name-set", MrmRtypeColor, colour, size, &applied) == MrmSUCCESS);
    CHECK(applied == 1);
    size = make_colour(colour, URMColorDescTypeRGB, sizeof(RGBColor), 0x10);
    CHECK(fetch_colour(dir, "valid-rgb", colour, size) == MrmSUCCESS);
}

static const struct {
    const char *name;
    void (*run)(const char *dir);
} cases[] = {
    { "short-stream", short_stream },
    { "truncated-header", truncated_header },
    { "short-stream-fetch-literal", short_stream_fetch_literal },
    { "valid-stream", valid_stream },
    { "colour-name-not-terminated", colour_name_not_terminated },
    { "colour-rgb-short", colour_rgb_short },
    { "valid-colour", valid_colour },
};

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s CASE TMPDIR\n", argv[0]);
        return 2;
    }
    for (size_t i = 0; i < sizeof cases / sizeof *cases; i++)
        if (strcmp(argv[1], cases[i].name) == 0) {
            MrmInitialize();
            cases[i].run(argv[2]);
            printf("PASS %s\n", cases[i].name);
            return 0;
        }
    fprintf(stderr, "unknown case %s\n", argv[1]);
    return 2;
}
