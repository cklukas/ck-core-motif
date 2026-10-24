/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/TextUnicode.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unicode/ubrk.h>
#include <unicode/utext.h>
#include <unicode/utf8.h>

static const char *stage = "startup";
static size_t position, assertions;
static void require(int condition, const char *detail)
{
    ++assertions;
    if (!condition) {
        fprintf(stderr, "incremental map: %s at %lu: %s\n", stage,
                (unsigned long)position, detail);
        exit(1);
    }
}

/* Unlike the fresh map, this oracle never partitions the input. ICU sees the
 * complete UTF-8 string, preserving a reference to whole-input UAX #29 rules. */
static void whole_input(const char *source, const XmTextUnicodeMap *map)
{
    size_t bytes = strlen(source), scalar = 0, grapheme = 0, actual, i;
    int32_t offset = 0, boundary;
    unsigned char *boundaries = calloc(bytes + 1, 1);
    UErrorCode error = U_ZERO_ERROR;
    UText *text = utext_openUTF8(NULL, source, (int64_t)bytes, &error);
    UBreakIterator *iterator = ubrk_open(UBRK_CHARACTER, "root", NULL, 0, &error);
    require(boundaries && text && iterator && U_SUCCESS(error), "whole-input ICU setup");
    ubrk_setUText(iterator, text, &error);
    require(U_SUCCESS(error), "whole-input ICU binding");
    for (boundary = ubrk_first(iterator); boundary != UBRK_DONE;
         boundary = ubrk_next(iterator)) {
        while (offset < boundary) {
            UChar32 c;
            U8_NEXT(source, offset, (int32_t)bytes, c);
            require(c > 0, "whole-input reference decoding");
            ++scalar;
        }
        require(offset == boundary, "ICU boundary inside scalar");
        boundaries[scalar] = 1;
        require(XmTextUnicodeScalarAtGrapheme(map, grapheme, &actual) &&
                actual == scalar, "segmented map differs from whole-input ICU");
        ++grapheme;
    }
    require(grapheme > 0 && XmTextUnicodeGraphemeCount(map) == grapheme - 1 &&
            XmTextUnicodeScalarCount(map) == scalar, "whole-input ICU counts");
    offset = 0;
    for (i = 0; i <= scalar; ++i) {
        position = i;
        require(XmTextUnicodeByteAtScalar(map, i, &actual) && actual == (size_t)offset,
                "independent UTF8 scalar byte offset");
        require(XmTextUnicodeIsGraphemeBoundary(map, i) == (int)boundaries[i],
                "whole-input boundary membership");
        if (i < scalar) {
            UChar32 c;
            U8_NEXT(source, offset, (int32_t)bytes, c);
            require(c > 0, "independent scalar decoding");
        }
    }
    ubrk_close(iterator); utext_close(text); free(boundaries);
}

/* The oracle is an independently constructed whole-input map. Compare failed
 * lookups too, including byte offsets inside multibyte encodings and scalar
 * positions inside clusters; matching only legal caret positions misses bugs. */
static void equal_maps(const XmTextUnicodeMap *actual,
                       const XmTextUnicodeMap *fresh)
{
    size_t i, a, b, bytes, scalars, graphemes;
    int ar, br;
    require(actual != NULL && fresh != NULL, "map creation");
    bytes = XmTextUnicodeByteCount(fresh);
    scalars = XmTextUnicodeScalarCount(fresh);
    graphemes = XmTextUnicodeGraphemeCount(fresh);
    require(XmTextUnicodeByteCount(actual) == bytes, "byte count");
    require(XmTextUnicodeScalarCount(actual) == scalars, "scalar count");
    require(XmTextUnicodeGraphemeCount(actual) == graphemes, "grapheme count");
    for (i = 0; i <= bytes + 1; ++i) {
        position = i; a = b = SIZE_MAX;
        ar = XmTextUnicodeScalarAtByte(actual, i, &a);
        br = XmTextUnicodeScalarAtByte(fresh, i, &b);
        require(ar == br && (!ar || a == b), "byte to scalar");
    }
    for (i = 0; i <= scalars + 1; ++i) {
        position = i; a = b = SIZE_MAX;
        ar = XmTextUnicodeByteAtScalar(actual, i, &a);
        br = XmTextUnicodeByteAtScalar(fresh, i, &b);
        require(ar == br && (!ar || a == b), "scalar to byte");
        require(XmTextUnicodeIsGraphemeBoundary(actual, i) ==
                XmTextUnicodeIsGraphemeBoundary(fresh, i), "boundary membership");
        a = b = SIZE_MAX;
        ar = XmTextUnicodePreviousGrapheme(actual, i, &a);
        br = XmTextUnicodePreviousGrapheme(fresh, i, &b);
        require(ar == br && (!ar || a == b), "previous boundary");
        a = b = SIZE_MAX;
        ar = XmTextUnicodeNextGrapheme(actual, i, &a);
        br = XmTextUnicodeNextGrapheme(fresh, i, &b);
        require(ar == br && (!ar || a == b), "next boundary");
    }
    for (i = 0; i <= graphemes + 1; ++i) {
        position = i; a = b = SIZE_MAX;
        ar = XmTextUnicodeScalarAtGrapheme(actual, i, &a);
        br = XmTextUnicodeScalarAtGrapheme(fresh, i, &b);
        require(ar == br && (!ar || a == b), "grapheme to scalar");
    }
}

static char *copy_text(const char *text)
{
    size_t bytes = strlen(text);
    char *copy = malloc(bytes + 1);
    require(copy != NULL, "fixture source allocation");
    memcpy(copy, text, bytes + 1);
    return copy;
}

/* Build the reference splice using the OLD map's exact scalar/byte conversion,
 * then leave both the old source bytes and map unchanged until explicitly freed. */
static XmTextUnicodeMap *edit(const XmTextUnicodeMap *old, const char *source,
                             size_t first, size_t last, const char *insert,
                             char **result)
{
    size_t begin, end, bytes = strlen(source), added = strlen(insert), total;
    XmTextUnicodeMap *next, *fresh;
    require(XmTextUnicodeByteAtScalar(old, first, &begin) &&
            XmTextUnicodeByteAtScalar(old, last, &end), "reference edit range");
    total = bytes - (end - begin) + added;
    *result = malloc(total + 1);
    require(*result != NULL, "reference splice allocation");
    memcpy(*result, source, begin);
    memcpy(*result + begin, insert, added);
    memcpy(*result + begin + added, source + end, bytes - end + 1);
    next = XmTextUnicodeMapReplace(old, first, last, insert, added);
    fresh = XmTextUnicodeMapCreate(*result, total);
    equal_maps(next, fresh);
    whole_input(*result, next);
    XmTextUnicodeMapFree(fresh);
    return next;
}

static void one_edit(const char *name, const char *source,
                     size_t first, size_t last, const char *insert)
{
    XmTextUnicodeMap *old, *retained, *next, *fresh;
    char *result;
    stage = name;
    old = XmTextUnicodeMapCreate(source, strlen(source));
    require(old != NULL, "original map");
    whole_input(source, old);
    retained = XmTextUnicodeMapRetain(old);
    require(retained == old, "retain identity");
    next = edit(old, source, first, last, insert, &result);
    XmTextUnicodeMapFree(old);
    fresh = XmTextUnicodeMapCreate(source, strlen(source));
    equal_maps(retained, fresh);
    /* Releasing the replacement first must not invalidate the old snapshot. */
    XmTextUnicodeMapFree(next);
    equal_maps(retained, fresh);
    XmTextUnicodeMapFree(retained);
    XmTextUnicodeMapFree(fresh);
    free(result);
}

static void deterministic_edits(void)
{
    static const char *inserts[] = {
        "", "x", "\n", "\r", "\r\n", "\xcc\x81",
        "\xf0\x9f\x87\xa6", "\xe2\x80\x8d\xf0\x9f\x91\xa9",
        "\xd7\x90\xd8\xa8", "\xe2\x81\xa6x\xe2\x81\xa9"
    };
    char *source = copy_text("start\r\nA\xcc\x81\n"
        "\xf0\x9f\x87\xa6\xf0\x9f\x87\xa7\xf0\x9f\x87\xa8"
        "\n\xd7\x90\xd8\xa8 end\n");
    XmTextUnicodeMap *map = XmTextUnicodeMapCreate(source, strlen(source));
    uint32_t random = 0x65b321u;
    size_t round;
    require(map != NULL, "edit sequence initial map");
    for (round = 0; round < 48; ++round) {
        size_t count = XmTextUnicodeScalarCount(map), first, last;
        char *result;
        XmTextUnicodeMap *next, *retained, *fresh;
        stage = "deterministic edit sequence";
        random = random * 1664525u + 1013904223u;
        first = random % (count + 1);
        random = random * 1664525u + 1013904223u;
        last = first + random % ((count - first < 4 ? count - first : 4) + 1);
        retained = XmTextUnicodeMapRetain(map);
        require(retained == map, "sequence retain");
        next = edit(map, source, first, last,
                    inserts[round % (sizeof inserts / sizeof inserts[0])], &result);
        XmTextUnicodeMapFree(map);
        fresh = XmTextUnicodeMapCreate(source, strlen(source));
        equal_maps(retained, fresh);
        XmTextUnicodeMapFree(fresh);
        XmTextUnicodeMapFree(retained);
        /* Opposite destruction order: replacement survives the last old ref. */
        fresh = XmTextUnicodeMapCreate(result, strlen(result));
        equal_maps(next, fresh);
        XmTextUnicodeMapFree(fresh);
        free(source); source = result; map = next;
    }
    XmTextUnicodeMapFree(map); free(source);
}

static void invalid_inputs(void)
{
    const char *bad[] = {"\xc0\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80", "\xe2\x82"};
    XmTextUnicodeMap *map, *fresh, *empty;
    XmTextUnicodeMapUsage usage;
    size_t i, value;
    stage = "invalid input preservation";
    map = XmTextUnicodeMapCreate("a\r\nb", 4);
    fresh = XmTextUnicodeMapCreate("a\r\nb", 4);
    require(map && fresh, "invalid fixture maps");
    for (i = 0; i < sizeof bad / sizeof bad[0]; ++i)
        require(!XmTextUnicodeMapReplace(map, 1, 2, bad[i], strlen(bad[i])), "invalid UTF8 accepted");
    require(!XmTextUnicodeMapReplace(map, 1, 2, "a\0b", 3), "embedded NUL accepted");
    require(!XmTextUnicodeMapReplace(map, 1, 2, NULL, 1), "NULL nonempty input accepted");
    require(!XmTextUnicodeMapReplace(map, 3, 2, "", 0), "reversed interval accepted");
    require(!XmTextUnicodeMapReplace(map, 0, 5, "", 0), "past end accepted");
    require(!XmTextUnicodeMapReplace(map, SIZE_MAX, SIZE_MAX, "", 0), "overflowing interval accepted");
    /* Reject impossible lengths before inspecting the short source buffer. No
     * portable allocator-failure injection is exposed by the public API. */
    require(!XmTextUnicodeMapReplace(map, 0, 0, "x", (size_t)INT32_MAX + 1), "oversized source accepted");
    require(!XmTextUnicodeMapReplace(NULL, 0, 0, "", 0), "NULL map accepted");
    require(!XmTextUnicodeMapRetain(NULL), "NULL retain accepted");
    require(!XmTextUnicodeMapGetUsage(NULL, &usage) &&
            !XmTextUnicodeMapGetUsage(map, NULL), "invalid usage arguments accepted");
    require(!XmTextUnicodeByteAtScalar(map, 0, NULL) &&
            !XmTextUnicodeScalarAtByte(map, 0, NULL) &&
            !XmTextUnicodeScalarAtGrapheme(map, 0, NULL) &&
            !XmTextUnicodePreviousGrapheme(map, 0, NULL) &&
            !XmTextUnicodeNextGrapheme(map, 0, NULL), "NULL output accepted");
    require(!XmTextUnicodeByteAtScalar(NULL, 0, &value), "NULL lookup map accepted");
    equal_maps(map, fresh);
    empty = XmTextUnicodeMapReplace(map, 0, 4, NULL, 0);
    require(empty != NULL && XmTextUnicodeByteCount(empty) == 0, "NULL empty deletion refused");
    XmTextUnicodeMapFree(empty); XmTextUnicodeMapFree(map); XmTextUnicodeMapFree(fresh);
    XmTextUnicodeMapFree(NULL);
}

static void source_ownership(void)
{
    char *source = copy_text("a\nb\nc"), *insert = copy_text("e\xcc\x81");
    XmTextUnicodeMap *map, *next, *fresh;
    stage = "source buffer ownership";
    map = XmTextUnicodeMapCreate(source, strlen(source));
    require(map != NULL, "owned source map");
    memset(source, 'x', strlen(source)); free(source);
    next = XmTextUnicodeMapReplace(map, 2, 3, insert, strlen(insert));
    memset(insert, 'x', strlen(insert)); free(insert);
    fresh = XmTextUnicodeMapCreate("a\ne\xcc\x81\nc", 7);
    equal_maps(next, fresh);
    XmTextUnicodeMapFree(fresh); XmTextUnicodeMapFree(map);
    /* Further replacement must read owned paragraph bytes, not either released
     * caller buffer, even after the original map itself has been released. */
    map = XmTextUnicodeMapReplace(next, 0, 1, "z", 1);
    fresh = XmTextUnicodeMapCreate("z\ne\xcc\x81\nc", 7);
    equal_maps(map, fresh);
    XmTextUnicodeMapFree(fresh); XmTextUnicodeMapFree(next); XmTextUnicodeMapFree(map);
}

static void bounded_work(void)
{
    enum { BLOCK = 128, REPEATS = 8192 };
    char *source = malloc((size_t)BLOCK * REPEATS + 1), *result;
    XmTextUnicodeMap *map, *next, *fresh;
    XmTextUnicodeMapUsage usage;
    size_t i, at = (size_t)BLOCK * (REPEATS / 2) + 17;
    stage = "many paragraph bounded work";
    require(source != NULL, "large source allocation");
    memset(source, 'a', (size_t)BLOCK * REPEATS);
    for (i = 0; i < REPEATS; ++i) source[(i + 1) * BLOCK - 1] = '\n';
    source[(size_t)BLOCK * REPEATS] = 0;
    map = XmTextUnicodeMapCreate(source, (size_t)BLOCK * REPEATS);
    require(map && XmTextUnicodeMapGetUsage(map, &usage), "initial usage");
    require(usage.mapped_bytes == (size_t)BLOCK * REPEATS &&
            usage.mapped_scalars == (size_t)BLOCK * REPEATS &&
            usage.reused_paragraphs == 0, "initial map accounting");
    next = edit(map, source, at, at + 1, "e\xcc\x81", &result);
    require(XmTextUnicodeMapGetUsage(next, &usage), "replacement usage");
    require(usage.mapped_bytes >= BLOCK && usage.mapped_bytes <= 4 * BLOCK &&
            usage.mapped_scalars >= BLOCK && usage.mapped_scalars <= 4 * BLOCK &&
            usage.rebuilt_paragraphs >= 1 && usage.rebuilt_paragraphs <= 3 &&
            usage.reused_paragraphs >= REPEATS - 3, "local edit remapped unrelated paragraphs");
    printf("many-paragraph mapped_bytes=%lu mapped_scalars=%lu rebuilt=%lu reused=%lu\n",
           (unsigned long)usage.mapped_bytes, (unsigned long)usage.mapped_scalars,
           (unsigned long)usage.rebuilt_paragraphs, (unsigned long)usage.reused_paragraphs);
    require(XmTextUnicodeMapGetUsage(map, &usage) &&
            usage.mapped_bytes == (size_t)BLOCK * REPEATS &&
            usage.reused_paragraphs == 0, "replacement changed old usage record");
    XmTextUnicodeMapFree(map);
    fresh = XmTextUnicodeMapCreate(result, strlen(result)); equal_maps(next, fresh);
    XmTextUnicodeMapFree(fresh); XmTextUnicodeMapFree(next); free(source); free(result);

    /* A hard paragraph can carry bidi/RI context across its entire length.
     * Rebuilding that whole paragraph is permitted and accounted honestly. */
    stage = "long single paragraph context";
    source = malloc(65536 + 1); require(source != NULL, "long source allocation");
    memset(source, 'x', 65536); source[65536] = 0;
    memcpy(source, "\xd7\x90\xd8\xa8", 4);
    map = XmTextUnicodeMapCreate(source, 65536);
    require(map != NULL, "long paragraph map");
    next = edit(map, source, 32000, 32001, "\xf0\x9f\x87\xa6", &result);
    require(XmTextUnicodeMapGetUsage(next, &usage) && usage.reused_paragraphs == 0 &&
            usage.rebuilt_paragraphs == 1 && usage.mapped_bytes == strlen(result) &&
            usage.mapped_scalars == XmTextUnicodeScalarCount(next), "single paragraph work accounting");
    XmTextUnicodeMapFree(next); XmTextUnicodeMapFree(map); free(source); free(result);
}

int main(void)
{
    require(XmTextUnicodeAvailable(), "Unicode support unavailable");
    one_edit("empty insertion", "", 0, 0, "e\xcc\x81");
    one_edit("complete deletion", "a\r\nb", 0, 4, "");
    one_edit("no-op replacement", "a\nb\n", 2, 2, "");
    one_edit("CRLF split", "a\r\nb\n", 2, 2, "x");
    one_edit("CRLF join", "a\rx\nb\n", 2, 3, "");
    one_edit("paragraph join", "a\nb\nc", 1, 2, "");
    one_edit("paragraph split", "abc", 1, 1, "\r\n");
    one_edit("paragraph boundary end", "a\nb\nc", 0, 2, "x\n");
    one_edit("RI parity insertion", "\xf0\x9f\x87\xa6\xf0\x9f\x87\xa7\xf0\x9f\x87\xa8\xf0\x9f\x87\xa9\xf0\x9f\x87\xaa", 1, 1, "\xf0\x9f\x87\xab");
    one_edit("RI parity deletion", "\xf0\x9f\x87\xa6\xf0\x9f\x87\xa7\xf0\x9f\x87\xa8\xf0\x9f\x87\xa9", 0, 1, "");
    one_edit("combining insertion", "ab", 1, 1, "\xcc\x81");
    one_edit("combining base deletion", "ae\xcc\x81z", 1, 2, "");
    one_edit("ZWJ join", "\xf0\x9f\x91\xa9\xf0\x9f\x91\xa7", 1, 1, "\xe2\x80\x8d");
    one_edit("ZWJ split", "\xf0\x9f\x91\xa9\xe2\x80\x8d\xf0\x9f\x91\xa7", 1, 2, "");
    one_edit("bidi isolate edit", "ab\xe2\x81\xa6\xd7\x90\xd8\xa8\xe2\x81\xa9z", 3, 5, "\xd8\xad\xd7\x91");
    deterministic_edits(); invalid_inputs(); source_ownership(); bounded_work();
    printf("incremental map assertions=%lu\n", (unsigned long)assertions);
    return 0;
}
