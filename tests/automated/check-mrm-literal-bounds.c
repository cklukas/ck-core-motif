/*
 * UID file reader: units literals that are shorter than their record, and data
 * entry offsets that are not aligned.  Run by run-mrm-literal-bounds.py with
 * the production lib/Mrm sources compiled under AddressSanitizer and
 * UndefinedBehaviorSanitizer.
 *
 *   check-mrm-literal-bounds CASE TMPDIR
 *
 * The UID files are written with the resource manager's own writer, the way the
 * UIL compiler writes them, and read back with MrmFetchSetValues and the
 * data entry functions.  The widget library is the stand-in of
 * tests/fuzz/mrm-stubs.c: it records the arguments of XtSetValues, and a unit
 * conversion keeps the value.
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

/* ---- units literals ---- */

/* Writes a file with the single literal "literal", fetches it as the width of a widget.
 * Returns the status of MrmFetchSetValues; *VALUE is the value that reached the widget,
 * and the number of arguments that reached it is returned in *APPLIED. */
static Cardinal fetch_literal(const char *dir, const char *name, MrmType type, const void *data, size_t size,
                              long *value, Cardinal *applied)
{
    char *path = uid_path(dir, name);
    IDBFile file = open_for_writing(path);
    MrmHierarchy hierarchy = NULL;
    String files[1] = { path };
    Widget parent = fuzz_new_widget(NULL);
    Widget widget = fuzz_new_widget(parent);
    Arg arg;
    Cardinal status;

    put_literal(file, "literal", type, data, size);
    CHECK(UrmIdbCloseFile(file, TRUE) == MrmSUCCESS);
    CHECK(MrmOpenHierarchyPerDisplay(fuzz_display(), 1, files, NULL, &hierarchy) == MrmSUCCESS);

    XtSetArg(arg, "width", (XtArgVal) "literal");
    fuzz_set_values_count = 0;
    status = MrmFetchSetValues(hierarchy, widget, &arg, 1);
    *applied = fuzz_set_values_count;
    *value = fuzz_set_values_count ? (long) fuzz_set_values_args[0].value : -1;
    MrmCloseHierarchy(hierarchy);
    fuzz_free_widgets();
    unlink(path);
    return status;
}

static void expect_rejected(const char *dir, const char *name, MrmType type, size_t size)
{
    unsigned char *bytes = calloc(1, size ? size : 1);
    long value;
    Cardinal applied;
    Cardinal status;

    CHECK(bytes != NULL);
    memset(bytes, 0x40, size);          /* a non-zero value: the units are then read */
    status = fetch_literal(dir, name, type, bytes, size, &value, &applied);
    CHECK(status != MrmSUCCESS);
    CHECK(applied == 0);
    free(bytes);
}

static void float_literal_too_short(const char *dir)
{
    /* RGMUnitsFloat is read as a whole: the double, then the units */
    expect_rejected(dir, "hfloat-8", MrmRtypeHorizontalFloat, 8);
    expect_rejected(dir, "vfloat-8", MrmRtypeVerticalFloat, 8);
    expect_rejected(dir, "hfloat-short", MrmRtypeHorizontalFloat, sizeof(RGMUnitsFloat) - 1);
    expect_rejected(dir, "vfloat-short", MrmRtypeVerticalFloat, sizeof(RGMUnitsFloat) - 1);
}

static void integer_literal_too_short(const char *dir)
{
    expect_rejected(dir, "hint-4", MrmRtypeHorizontalInteger, sizeof(RGMUnitsInteger) - 4);
    expect_rejected(dir, "vint-4", MrmRtypeVerticalInteger, sizeof(RGMUnitsInteger) - 4);
}

/* MrmFetchLiteral converts the same literals without a widget. */
static void fetch_literal_too_short(const char *dir)
{
    char *path = uid_path(dir, "fetch-literal");
    IDBFile file = open_for_writing(path);
    MrmHierarchy hierarchy = NULL;
    String files[1] = { path };
    unsigned char bytes[sizeof(RGMUnitsFloat)];
    XtPointer value = (XtPointer) &bytes;
    MrmCode type = 0;

    memset(bytes, 0x40, sizeof bytes);
    put_literal(file, "short-float", MrmRtypeHorizontalFloat, bytes, 8);
    put_literal(file, "short-integer", MrmRtypeVerticalInteger, bytes, 4);
    CHECK(UrmIdbCloseFile(file, TRUE) == MrmSUCCESS);
    CHECK(MrmOpenHierarchyPerDisplay(fuzz_display(), 1, files, NULL, &hierarchy) == MrmSUCCESS);
    CHECK(MrmFetchLiteral(hierarchy, "short-float", fuzz_display(), &value, &type) != MrmSUCCESS);
    CHECK(value == NULL && type == 0);
    value = (XtPointer) &bytes;
    CHECK(MrmFetchLiteral(hierarchy, "short-integer", fuzz_display(), &value, &type) != MrmSUCCESS);
    CHECK(value == NULL && type == 0);
    MrmCloseHierarchy(hierarchy);
    unlink(path);
}

static void fetch_literal_valid(const char *dir)
{
    char *path = uid_path(dir, "fetch-literal-valid");
    IDBFile file = open_for_writing(path);
    MrmHierarchy hierarchy = NULL;
    String files[1] = { path };
    RGMUnitsFloat record;
    double number = 4.0;
    XtPointer value = NULL;
    MrmCode type = 0;

    memset(&record, 0, sizeof record);
    memcpy(record.value, &number, sizeof number);
    record.units = XmPIXELS;
    put_literal(file, "float", MrmRtypeVerticalFloat, &record, sizeof record);
    CHECK(UrmIdbCloseFile(file, TRUE) == MrmSUCCESS);
    CHECK(MrmOpenHierarchyPerDisplay(fuzz_display(), 1, files, NULL, &hierarchy) == MrmSUCCESS);
    CHECK(MrmFetchLiteral(hierarchy, "float", fuzz_display(), &value, &type) == MrmSUCCESS);
    CHECK(type == MrmRtypeVerticalFloat && (long) value == 4);
    MrmCloseHierarchy(hierarchy);
    unlink(path);
}

static void float_literal_valid(const char *dir)
{
    RGMUnitsFloat record;
    double value = 3.0;
    long result;
    Cardinal applied;

    memset(&record, 0, sizeof record);
    memcpy(record.value, &value, sizeof value);
    record.units = XmPIXELS;
    CHECK(fetch_literal(dir, "hfloat", MrmRtypeHorizontalFloat, &record, sizeof record, &result, &applied) ==
          MrmSUCCESS);
    CHECK(applied == 1 && result == 3);
    value = 7.0;
    memcpy(record.value, &value, sizeof value);
    CHECK(fetch_literal(dir, "vfloat", MrmRtypeVerticalFloat, &record, sizeof record, &result, &applied) ==
          MrmSUCCESS);
    CHECK(applied == 1 && result == 7);
}

static void integer_literal_valid(const char *dir)
{
    RGMUnitsInteger record = { 5, XmPIXELS };
    long result;
    Cardinal applied;

    CHECK(fetch_literal(dir, "hint", MrmRtypeHorizontalInteger, &record, sizeof record, &result, &applied) ==
          MrmSUCCESS);
    CHECK(applied == 1 && result == 5);
    record.value = 9;
    CHECK(fetch_literal(dir, "vint", MrmRtypeVerticalInteger, &record, sizeof record, &result, &applied) ==
          MrmSUCCESS);
    CHECK(applied == 1 && result == 9);
}

/* ---- data entry offsets ---- */

enum { ENTRY_SIZE = 301 };          /* not a multiple of four: the writer pads each entry to a full word */

static void fill_pattern(unsigned char *bytes, int serial)
{
    for (int i = 0; i < ENTRY_SIZE; i++) bytes[i] = (unsigned char) (serial * 31 + i);
}

/* Stores two entries in the header record, and entries through the general entry writer until
 * two of them sit in a data record (the writer fills the header record first).  SERIAL
 * numbers the contents of the entries of the data records. */
static void store_entries(IDBFile file, IDBDataHandle data[2], int data_serial[2], IDBDataHandle header[2])
{
    unsigned char bytes[ENTRY_SIZE];
    int found = 0;

    for (int serial = 0; serial < 200 && found < 2; serial++) {
        IDBDataHandle handle;
        URMResourceContextPtr context;

        fill_pattern(bytes, serial);
        context = filled_context(MrmRtypeChar8, bytes, sizeof bytes);
        CHECK(Idb__DB_PutDataEntry(file, context, &handle) == MrmSUCCESS);
        if (serial == 0 || serial == 1) {
            /* the header record takes the first two */
            CHECK(Idb__HDR_PutDataEntry(file, context, &header[serial]) == MrmSUCCESS);
        }
        if (handle.rec_no != IDBHeaderRecordNumber) {
            data[found] = handle;
            data_serial[found++] = serial;
        }
        UrmFreeResourceContext(context);
    }
    CHECK(found == 2);
}

static void entry_matches(IDBFile file, IDBDataHandle handle, Boolean header, int serial)
{
    URMResourceContextPtr context = new_context(8);
    unsigned char bytes[ENTRY_SIZE];

    fill_pattern(bytes, serial);
    CHECK((header ? Idb__HDR_GetDataEntry(file, handle, context)
                  : Idb__DB_GetDataEntry(file, handle, context)) == MrmSUCCESS);
    CHECK(UrmRCSize(context) == ENTRY_SIZE);
    CHECK(memcmp(UrmRCBuffer(context), bytes, ENTRY_SIZE) == 0);
    UrmFreeResourceContext(context);
}

static void unaligned_entries(const char *dir, Boolean header)
{
    char *path = uid_path(dir, header ? "header-entry" : "data-entry");
    IDBFile file;
    IDBDataHandle data[2], hdr[2];
    int serial[2];
    IDBDataHandle *handles;
    URMResourceContextPtr context;

    file = open_for_writing(path);
    store_entries(file, data, serial, hdr);
    handles = header ? hdr : data;
    CHECK(header ? handles[0].rec_no == IDBHeaderRecordNumber : handles[0].rec_no != IDBHeaderRecordNumber);
    CHECK(handles[0].item_offs % 4 == 0 && handles[1].item_offs % 4 == 0);

    for (int shift = 1; shift <= 3; shift++) {
        IDBDataHandle bad = handles[0];

        bad.item_offs = (MrmOffset) (bad.item_offs + shift);
        context = new_context(8);
        CHECK((header ? Idb__HDR_GetDataEntry(file, bad, context)
                      : Idb__DB_GetDataEntry(file, bad, context)) != MrmSUCCESS);
        UrmFreeResourceContext(context);
        CHECK(!(header ? Idb__HDR_MatchFilter(file, bad, URMgNul, URMtNul)
                       : Idb__DB_MatchFilter(file, bad, URMgNul, URMtNul)));
    }
    CHECK(UrmIdbCloseFile(file, FALSE) == MrmSUCCESS);
    unlink(path);
}

static void unaligned_data_entry(const char *dir) { unaligned_entries(dir, FALSE); }
static void unaligned_header_entry(const char *dir) { unaligned_entries(dir, TRUE); }

static void aligned_entries(const char *dir)
{
    char *path = uid_path(dir, "aligned-entries");
    IDBFile file;
    IDBDataHandle data[2], hdr[2];
    int serial[2];

    file = open_for_writing(path);
    store_entries(file, data, serial, hdr);
    entry_matches(file, data[0], FALSE, serial[0]);
    entry_matches(file, data[1], FALSE, serial[1]);
    entry_matches(file, hdr[0], TRUE, 0);
    entry_matches(file, hdr[1], TRUE, 1);
    CHECK(Idb__DB_MatchFilter(file, data[1], URMgNul, URMtNul));
    CHECK(Idb__HDR_MatchFilter(file, hdr[1], URMgNul, URMtNul));
    CHECK(UrmIdbCloseFile(file, FALSE) == MrmSUCCESS);
    unlink(path);
}

static const struct {
    const char *name;
    void (*run)(const char *dir);
} cases[] = {
    { "float-literal-too-short", float_literal_too_short },
    { "integer-literal-too-short", integer_literal_too_short },
    { "fetch-literal-too-short", fetch_literal_too_short },
    { "unaligned-data-entry", unaligned_data_entry },
    { "unaligned-header-entry", unaligned_header_entry },
    { "valid-float-literal", float_literal_valid },
    { "valid-integer-literal", integer_literal_valid },
    { "valid-fetch-literal", fetch_literal_valid },
    { "valid-aligned-entries", aligned_entries },
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
