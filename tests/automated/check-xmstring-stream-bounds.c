/*
 * Compound string external format: component bounds, the tag cache and the
 * segment reader.  Run by run-xmstring-stream-bounds.py with the production
 * lib/Xm sources compiled under AddressSanitizer and UndefinedBehaviorSanitizer.
 *
 *   check-xmstring-stream-bounds CASE [TMPDIR]
 *
 * Every stream that is parsed is held in a heap block of exactly the length
 * that its header declares, so a component that is read beyond the stream is an
 * address error.  A case exits 0 when it passes.
 */
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <Xm/XmP.h>
#include <Xm/XmStringI.h>

/* The toolkit headers redefine assert(), so the checks use their own macro. */
#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #condition); \
            abort(); \
        } \
    } while (0)


static unsigned char *exact_copy(const unsigned char *bytes, size_t size)
{
    unsigned char *copy = malloc(size);
    CHECK(copy != NULL);
    memcpy(copy, bytes, size);
    return copy;
}

/* Parses BYTES (a complete stream) in an exact-size heap block. */
static XmString parse(const unsigned char *bytes, size_t size)
{
    unsigned char *copy = exact_copy(bytes, size);
    XmString string = XmCvtByteStreamToXmString(copy);
    free(copy);
    return string;
}

/* The converted string must be released, and must survive a walk of its triples. */
static void walk_triples(XmString string)
{
    XmStringContext context;
    XtPointer value;
    unsigned int length;
    int guard = 0;

    CHECK(XmStringInitContext(&context, string));
    while (XmStringGetNextTriple(context, &length, &value) != XmSTRING_COMPONENT_END &&
           guard++ < 1000)
        XtFree(value);
    XmStringFreeContext(context);
}

static void expect_rejected(const unsigned char *bytes, size_t size)
{
    XmString string = parse(bytes, size);
    CHECK(string == NULL);
}

/* ---- the streams that were read beyond their end ---- */

/* A separator, which cannot be optimized, then a tag component that claims 39 bytes. */
static void nonopt_length(void)
{
    static const unsigned char stream[] = { 0xdf, 0x80, 0x06, 0x07, 0x04, 0x00, 0x06, 0x27, 'a', 'b', 'c' };
    expect_rejected(stream, sizeof stream);
}

/* The second component is a tag byte without its length. */
static void nonopt_missing_length(void)
{
    static const unsigned char stream[] = { 0xdf, 0x80, 0x06, 0x03, 0x04, 0x00, 0x06 };
    expect_rejected(stream, sizeof stream);
}

/* A long-form length (two more bytes) that is cut off by the end of the stream. */
static void nonopt_truncated_long_length(void)
{
    static const unsigned char stream[] = { 0xdf, 0x80, 0x06, 0x04, 0x04, 0x00, 0x02, 0x82 };
    expect_rejected(stream, sizeof stream);
}

/* A direction component whose one byte value is missing. */
static void direction_without_value(void)
{
    static const unsigned char stream[] = { 0xdf, 0x80, 0x06, 0x02, 0x03, 0x00 };
    expect_rejected(stream, sizeof stream);
}

/* A layout push (not optimizable) whose one byte value is missing. */
static void push_without_value(void)
{
    static const unsigned char stream[] = { 0xdf, 0x80, 0x06, 0x04, 0x04, 0x00, 0x08, 0x00 };
    expect_rejected(stream, sizeof stream);
}

/* ---- the tag cache ---- */

static void cache_embedded_nul(void)
{
    static const char embedded[] = { 'a', 'b', '\0', 'x', 'y', 'z', 'z', 'y' };
    int first = _XmStringIndexCacheTag("ab", 2);
    int again = _XmStringIndexCacheTag((char *) embedded, (int) sizeof embedded);

    /* a counted tag ends at its first NUL: it is the tag "ab" */
    CHECK(first > 0);
    CHECK(again == first);
    CHECK(strcmp(_XmStringIndexGetTag(again), "ab") == 0);
    CHECK(_XmStringIndexCacheTag("ab", XmSTRING_TAG_STRLEN) == first);
}

/* ---- the segment reader ---- */

/* The cached tag of NAME must still be there: the tag cache owns it, not the caller. */
static void expect_cached_tag(const char *name)
{
    int index = _XmStringIndexCacheTag((char *) name, XmSTRING_TAG_STRLEN);

    CHECK(strcmp(_XmStringIndexGetTag(index), name) == 0);
}

static void next_segment_all_failures(XmString string)
{
    XmStringContext context;
    char *text;
    XmStringTag tag;
    XmStringDirection direction;
    Boolean separator;
    int guard = 0;

    CHECK(XmStringInitContext(&context, string));
    while (XmStringGetNextSegment(context, &text, &tag, &direction, &separator) && guard++ < 100) {
        XtFree(text);
        XtFree(tag);
    }
    /* a failed read hands nothing out */
    CHECK(text == NULL);
    CHECK(tag == NULL);
    XmStringFreeContext(context);
}

/* The 8-byte stream of the fuzzer: components of tag 0 and length 0. */
static void next_segment_borrowed_tag(void)
{
    static const unsigned char stream[] = { 0xdf, 0x80, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00 };
    XmString string = parse(stream, sizeof stream);

    if (string) {
        next_segment_all_failures(string);
        XmStringFree(string);
    }
}

/* A segment of more than SHRT_MAX bytes cannot be returned: the text belongs to the string. */
static void next_segment_oversized_text(void)
{
    size_t size = (size_t) SHRT_MAX + 100;
    char *text = malloc(size + 1);
    XmString string;

    CHECK(text != NULL);
    memset(text, 'x', size);
    text[size] = '\0';
    string = XmStringCreate(text, "tag");
    CHECK(string != NULL);
    next_segment_all_failures(string);
    expect_cached_tag("tag");
    XmStringFree(string);
    free(text);
}

/* A wide character component of one byte: the copy is of that many bytes, not of wide characters. */
static void next_segment_wide_text_odd_length(void)
{
    static const unsigned char stream[] = { 0xdf, 0x80, 0x06, 0x05, 0x07, 0x01, 0x00, 0x00, 0x00 };
    XmString string = parse(stream, sizeof stream);

    if (string) {
        next_segment_all_failures(string);
        XmStringFree(string);
    }
}

/* Wide character text is returned as multibyte text. */
static void next_segment_wide_text(void)
{
    static const wchar_t wide[] = L"wide";
    unsigned char stream[6 + sizeof wide - sizeof(wchar_t)] = { 0xdf, 0x80, 0x06, 0, 0x07, 0 };
    size_t bytes = sizeof wide - sizeof(wchar_t);
    XmString string;
    XmStringContext context;
    char *text;
    XmStringTag tag;
    XmStringDirection direction;
    Boolean separator;

    stream[3] = (unsigned char) (bytes + 2);
    stream[5] = (unsigned char) bytes;
    memcpy(stream + 6, wide, bytes);
    string = parse(stream, 6 + bytes);
    CHECK(string != NULL);
    CHECK(XmStringInitContext(&context, string));
    CHECK(XmStringGetNextSegment(context, &text, &tag, &direction, &separator));
    CHECK(strcmp(text, "wide") == 0);
    XtFree(text);
    XtFree(tag);
    XmStringFreeContext(context);
    XmStringFree(string);
}

/* ---- streams that must keep working ---- */

static unsigned char *to_stream(XmString string, unsigned int *size)
{
    unsigned char *stream = NULL;

    *size = XmCvtXmStringToByteStream(string, &stream);
    CHECK(stream != NULL && *size > 0);
    CHECK(XmStringByteStreamLength(stream) == *size);
    return stream;
}

static void roundtrip(XmString original)
{
    unsigned int size;
    unsigned char *stream = to_stream(original, &size);
    XmString parsed = parse(stream, size);

    CHECK(parsed != NULL);
    CHECK(XmStringByteCompare(original, parsed));
    walk_triples(parsed);
    XmStringFree(parsed);
    XtFree((char *) stream);
}

static void valid_single_segment(void)
{
    XmString string = XmStringCreate("hello", "tag");

    roundtrip(string);
    XmStringFree(string);
}

static void valid_unoptimizable(void)
{
    XmString first = XmStringCreate("hello", "tag");
    XmString second = XmStringCreate("world", "other");
    XmString separator = XmStringSeparatorCreate();
    XmString direction = XmStringDirectionCreate(XmSTRING_DIRECTION_R_TO_L);
    XmString tab = XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0, NULL);
    XmString parts[] = { first, separator, direction, tab, second };
    XmString joined = NULL;

    for (size_t i = 0; i < sizeof parts / sizeof *parts; i++) {
        XmString next = XmStringConcat(joined ? joined : parts[i], joined ? parts[i] : XmStringCreate("", "tag"));
        if (joined) XmStringFree(joined);
        joined = next;
    }
    CHECK(joined != NULL);
    CHECK(XmStringLineCount(joined) == 2);
    roundtrip(joined);
    XmStringFree(joined);
    for (size_t i = 0; i < sizeof parts / sizeof *parts; i++) XmStringFree(parts[i]);
}

static void valid_segments(void)
{
    XmString first = XmStringCreate("hello", "tag");
    XmString second = XmStringCreate("world", "other");
    XmString joined = XmStringConcat(first, second);
    XmStringContext context;
    char *text;
    XmStringTag tag;
    XmStringDirection direction;
    Boolean separator;

    CHECK(XmStringInitContext(&context, joined));
    CHECK(XmStringGetNextSegment(context, &text, &tag, &direction, &separator));
    CHECK(strcmp(text, "hello") == 0 && strcmp(tag, "tag") == 0);
    XtFree(text);
    XtFree(tag);
    CHECK(XmStringGetNextSegment(context, &text, &tag, &direction, &separator));
    CHECK(strcmp(text, "world") == 0 && strcmp(tag, "other") == 0);
    XtFree(text);
    XtFree(tag);
    CHECK(!XmStringGetNextSegment(context, &text, &tag, &direction, &separator));
    XmStringFreeContext(context);
    XmStringFree(joined);
    XmStringFree(first);
    XmStringFree(second);
}

static void valid_cache_tags(void)
{
    int ab = _XmStringIndexCacheTag("ab", 2);
    int abc = _XmStringIndexCacheTag("abc", 3);
    int prefix = _XmStringIndexCacheTag("abcd", 2);
    int empty = _XmStringIndexCacheTag("", 0);

    CHECK(ab > 0 && abc > 0 && ab != abc);
    CHECK(prefix == ab);
    CHECK(_XmStringIndexCacheTag("abc", XmSTRING_TAG_STRLEN) == abc);
    CHECK(_XmStringIndexCacheTag("ab", XmSTRING_TAG_STRLEN) == ab);
    CHECK(_XmStringIndexCacheTag("", 0) == empty && empty != ab);
    CHECK(strcmp(_XmStringIndexGetTag(abc), "abc") == 0);
}

static const struct {
    const char *name;
    void (*run)(void);
} cases[] = {
    { "nonopt-length", nonopt_length },
    { "nonopt-missing-length", nonopt_missing_length },
    { "nonopt-truncated-long-length", nonopt_truncated_long_length },
    { "direction-without-value", direction_without_value },
    { "push-without-value", push_without_value },
    { "cache-embedded-nul", cache_embedded_nul },
    { "next-segment-borrowed-tag", next_segment_borrowed_tag },
    { "next-segment-oversized-text", next_segment_oversized_text },
    { "next-segment-wide-text-odd-length", next_segment_wide_text_odd_length },
    { "next-segment-wide-text", next_segment_wide_text },
    { "valid-single-segment", valid_single_segment },
    { "valid-unoptimizable", valid_unoptimizable },
    { "valid-segments", valid_segments },
    { "valid-cache-tags", valid_cache_tags },
};

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "C");
    if (argc < 2) {
        fprintf(stderr, "usage: %s CASE [TMPDIR]\n", argv[0]);
        return 2;
    }
    for (size_t i = 0; i < sizeof cases / sizeof *cases; i++)
        if (strcmp(argv[1], cases[i].name) == 0) {
            cases[i].run();
            printf("PASS %s\n", cases[i].name);
            return 0;
        }
    fprintf(stderr, "unknown case %s\n", argv[1]);
    return 2;
}
