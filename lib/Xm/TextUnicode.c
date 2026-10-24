/* SPDX-License-Identifier: LGPL-2.0-or-later */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "TextUnicode.h"
#include <ctype.h>
#include <langinfo.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNICODE_TEXT
#include <unicode/ubrk.h>
#include <unicode/utext.h>
#include <unicode/utf8.h>
#endif
/* Bodies own UTF-8 and local maps. Metadata alone is copied between versions. */
typedef struct {
    size_t references, bytes, scalars, graphemes;
    char *utf8;
    size_t *scalar_bytes, *grapheme_scalars;
} Paragraph;
typedef struct {
    Paragraph *body;
    size_t byte, scalar, grapheme;
} ParagraphEntry;
struct _XmTextUnicodeMap {
    size_t references, bytes, scalars, graphemes, count;
    ParagraphEntry *paragraphs;
    XmTextUnicodeMapUsage usage;
};
int XmTextUnicodeAvailable(void) {
#ifdef HAVE_UNICODE_TEXT
    return 1;
#else
    return 0;
#endif
}
int XmTextUnicodeLocaleAvailable(void) {
#ifdef HAVE_UNICODE_TEXT
    const char *code = nl_langinfo(CODESET);
    if (!code || tolower((unsigned char)code[0]) != 'u') return 0;
    if (!code[1] || tolower((unsigned char)code[1]) != 't') return 0;
    if (!code[2] || tolower((unsigned char)code[2]) != 'f') return 0;
    code += 3;
    if (*code == '-') ++code;
    return code[0] == '8' && code[1] == 0;
#else
    return 0;
#endif
}
static void paragraph_free(Paragraph *p) {
    if (p && --p->references == 0) {
        free(p->utf8); free(p->scalar_bytes); free(p->grapheme_scalars); free(p);
    }
}
XmTextUnicodeMap *XmTextUnicodeMapRetain(XmTextUnicodeMap *map) {
    if (!map || map->references == SIZE_MAX) return NULL;
    ++map->references;
    return map;
}
void XmTextUnicodeMapFree(XmTextUnicodeMap *map) {
    if (map && --map->references == 0) {
        size_t n;
        for (n = 0; n < map->count; ++n) paragraph_free(map->paragraphs[n].body);
        free(map->paragraphs); free(map);
    }
}
int XmTextUnicodeMapGetUsage(const XmTextUnicodeMap *map, XmTextUnicodeMapUsage *usage) {
    if (!map || !usage) return 0;
    *usage = map->usage; return 1;
}
static size_t lower_bound(const size_t *values, size_t count, size_t value) {
    size_t first = 0;
    while (first < count) {
        size_t middle = first + (count - first) / 2;
        if (values[middle] < value) first = middle + 1;
        else count = middle;
    }
    return first;
}
#ifdef HAVE_UNICODE_TEXT
static Paragraph *paragraph_create(const char *utf8, size_t length) {
    Paragraph *p;
    int32_t offset = 0, boundary;
    UErrorCode error = U_ZERO_ERROR;
    UBreakIterator *iterator;
    UText *text;
    if (length > SIZE_MAX / sizeof(size_t) - 1) return NULL;
    p = (Paragraph *)calloc(1, sizeof(*p));
    if (!p) return NULL;
    p->references = 1; p->bytes = length;
    p->utf8 = (char *)malloc(length + 1);
    p->scalar_bytes = (size_t *)malloc((length + 1) * sizeof(size_t));
    p->grapheme_scalars = (size_t *)malloc((length + 1) * sizeof(size_t));
    if (!p->utf8 || !p->scalar_bytes || !p->grapheme_scalars) { paragraph_free(p); return NULL; }
    if (length) memcpy(p->utf8, utf8, length);
    p->utf8[length] = 0;
    while ((size_t)offset < length) {
        UChar32 scalar;
        p->scalar_bytes[p->scalars++] = (size_t)offset;
        U8_NEXT(p->utf8, offset, (int32_t)length, scalar);
        if (scalar <= 0) { paragraph_free(p); return NULL; }
    }
    p->scalar_bytes[p->scalars] = length;
    text = utext_openUTF8(NULL, p->utf8, (int64_t)length, &error);
    iterator = ubrk_open(UBRK_CHARACTER, "root", NULL, 0, &error);
    if (U_FAILURE(error) || !text || !iterator) {
        if (text) utext_close(text);
        if (iterator) ubrk_close(iterator);
        paragraph_free(p); return NULL;
    }
    ubrk_setUText(iterator, text, &error);
    if (U_FAILURE(error)) { utext_close(text); ubrk_close(iterator); paragraph_free(p); return NULL; }
    for (boundary = ubrk_first(iterator); boundary != UBRK_DONE; boundary = ubrk_next(iterator)) {
        size_t scalar = lower_bound(p->scalar_bytes, p->scalars + 1, (size_t)boundary);
        if (scalar > p->scalars || p->scalar_bytes[scalar] != (size_t)boundary) {
            utext_close(text); ubrk_close(iterator); paragraph_free(p); return NULL;
        }
        p->grapheme_scalars[p->graphemes++] = scalar;
    }
    if (p->graphemes) --p->graphemes;
    utext_close(text); ubrk_close(iterator);
    return p;
}
static XmTextUnicodeMap *map_allocate(size_t count) {
    XmTextUnicodeMap *map;
    if (!count || count > SIZE_MAX / sizeof(ParagraphEntry)) return NULL;
    map = (XmTextUnicodeMap *)calloc(1, sizeof(*map));
    if (!map) return NULL;
    map->references = 1;
    map->paragraphs = (ParagraphEntry *)calloc(count, sizeof(ParagraphEntry));
    if (!map->paragraphs) { free(map); return NULL; }
    return map;
}
/* Transfer an owned body into an unpublished map. Total bytes <= INT32_MAX,
 * so scalar/grapheme prefix sums are bounded by the same limit. */
static void append_owned(XmTextUnicodeMap *map, Paragraph *body) {
    ParagraphEntry *entry = &map->paragraphs[map->count++];
    entry->body = body; entry->byte = map->bytes;
    entry->scalar = map->scalars; entry->grapheme = map->graphemes;
    map->bytes += body->bytes; map->scalars += body->scalars;
    map->graphemes += body->graphemes;
}
static int append_shared(XmTextUnicodeMap *map, Paragraph *body) {
    if (body->references == SIZE_MAX) return 0;
    ++body->references; append_owned(map, body); return 1;
}
#endif
XmTextUnicodeMap *XmTextUnicodeMapCreate(const char *utf8, size_t length) {
#ifdef HAVE_UNICODE_TEXT
    XmTextUnicodeMap *map;
    size_t count = 0, start = 0, n;
    if ((!utf8 && length) || length > INT32_MAX) return NULL;
    for (n = 0; n < length; ++n) if (utf8[n] == '\n') ++count;
    if (!length || utf8[length - 1] != '\n') ++count;
    map = map_allocate(count);
    if (!map) return NULL;
    for (n = 0; n <= length; ++n) {
        if ((n < length && utf8[n] == '\n') || (n == length && (start < length || !length))) {
            size_t end = n < length ? n + 1 : n;
            Paragraph *body = paragraph_create(utf8 ? utf8 + start : "", end - start);
            if (!body) { XmTextUnicodeMapFree(map); return NULL; }
            append_owned(map, body); start = end;
        }
    }
    map->usage.mapped_bytes = map->bytes;
    map->usage.mapped_scalars = map->scalars;
    map->usage.rebuilt_paragraphs = map->count;
    return map;
#else
    (void)utf8; (void)length; return NULL;
#endif
}
/* Find the last prefix <= coordinate, including the final body's endpoint. */
static size_t paragraph_at(const XmTextUnicodeMap *map, size_t coordinate, int kind) {
    size_t first = 0, last = map->count;
    while (first < last) {
        size_t middle = first + (last - first) / 2;
        const ParagraphEntry *e = &map->paragraphs[middle];
        size_t prefix = kind == 0 ? e->byte : kind == 1 ? e->scalar : e->grapheme;
        if (prefix <= coordinate) first = middle + 1;
        else last = middle;
    }
    return first ? first - 1 : 0;
}
XmTextUnicodeMap *XmTextUnicodeMapReplace(const XmTextUnicodeMap *map,
    size_t first_scalar, size_t last_scalar, const char *utf8, size_t bytes) {
#ifdef HAVE_UNICODE_TEXT
    size_t first, last, first_byte, last_byte, end_byte, left, right, length, count, n;
    XmTextUnicodeMap *rebuilt, *result;
    char *joined;
    if (!map || (!utf8 && bytes) || first_scalar > last_scalar ||
        last_scalar > map->scalars || bytes > INT32_MAX) return NULL;
    if (!XmTextUnicodeByteAtScalar(map, first_scalar, &first_byte) ||
        !XmTextUnicodeByteAtScalar(map, last_scalar, &last_byte) ||
        bytes > (size_t)INT32_MAX - (map->bytes - (last_byte - first_byte))) return NULL;
    first = paragraph_at(map, first_scalar, 1);
    last = paragraph_at(map, last_scalar, 1);
    end_byte = map->paragraphs[last].byte + map->paragraphs[last].body->bytes;
    left = first_byte - map->paragraphs[first].byte;
    right = end_byte - last_byte;
    length = left + bytes + right;
    joined = (char *)malloc(length + 1);
    if (!joined) return NULL;
    if (left) memcpy(joined, map->paragraphs[first].body->utf8, left);
    if (bytes) memcpy(joined + left, utf8, bytes);
    if (right) memcpy(joined + left + bytes,
        map->paragraphs[last].body->utf8 + last_byte - map->paragraphs[last].byte, right);
    joined[length] = 0;
    rebuilt = XmTextUnicodeMapCreate(joined, length); free(joined);
    if (!rebuilt) return NULL;
    count = rebuilt->count;
    /* Do not insert an artificial empty body between surviving paragraphs. */
    if (!length && (first || last + 1 < map->count)) count = 0;
    if (first > SIZE_MAX - count || first + count > SIZE_MAX - (map->count - last - 1)) {
        XmTextUnicodeMapFree(rebuilt); return NULL;
    }
    result = map_allocate(first + count + map->count - last - 1);
    if (!result) { XmTextUnicodeMapFree(rebuilt); return NULL; }
    result->usage = rebuilt->usage;
    result->usage.rebuilt_paragraphs = count;
    result->usage.reused_paragraphs = first + map->count - last - 1;
    for (n = 0; n < first; ++n)
        if (!append_shared(result, map->paragraphs[n].body)) goto failed;
    for (n = 0; n < count; ++n)
        if (!append_shared(result, rebuilt->paragraphs[n].body)) goto failed;
    for (n = last + 1; n < map->count; ++n)
        if (!append_shared(result, map->paragraphs[n].body)) goto failed;
    XmTextUnicodeMapFree(rebuilt); return result;
failed:
    XmTextUnicodeMapFree(rebuilt); XmTextUnicodeMapFree(result); return NULL;
#else
    (void)map; (void)first_scalar; (void)last_scalar; (void)utf8; (void)bytes;
    return NULL;
#endif
}
size_t XmTextUnicodeByteCount(const XmTextUnicodeMap *map) { return map ? map->bytes : 0; }
size_t XmTextUnicodeScalarCount(const XmTextUnicodeMap *map) { return map ? map->scalars : 0; }
size_t XmTextUnicodeGraphemeCount(const XmTextUnicodeMap *map) { return map ? map->graphemes : 0; }
int XmTextUnicodeByteAtScalar(const XmTextUnicodeMap *map, size_t scalar, size_t *byte) {
    const ParagraphEntry *e;
    if (!map || !byte || scalar > map->scalars) return 0;
    e = &map->paragraphs[paragraph_at(map, scalar, 1)];
    *byte = e->byte + e->body->scalar_bytes[scalar - e->scalar]; return 1;
}
int XmTextUnicodeScalarAtByte(const XmTextUnicodeMap *map, size_t byte, size_t *scalar) {
    const ParagraphEntry *e; size_t local, position;
    if (!map || !scalar || byte > map->bytes) return 0;
    e = &map->paragraphs[paragraph_at(map, byte, 0)]; local = byte - e->byte;
    position = lower_bound(e->body->scalar_bytes, e->body->scalars + 1, local);
    if (position > e->body->scalars || e->body->scalar_bytes[position] != local) return 0;
    *scalar = e->scalar + position; return 1;
}
int XmTextUnicodeScalarAtGrapheme(const XmTextUnicodeMap *map, size_t grapheme, size_t *scalar) {
    const ParagraphEntry *e;
    if (!map || !scalar || grapheme > map->graphemes) return 0;
    e = &map->paragraphs[paragraph_at(map, grapheme, 2)];
    *scalar = e->scalar + e->body->grapheme_scalars[grapheme - e->grapheme]; return 1;
}
int XmTextUnicodeIsGraphemeBoundary(const XmTextUnicodeMap *map, size_t scalar) {
    const ParagraphEntry *e; size_t local, position;
    if (!map || scalar > map->scalars) return 0;
    e = &map->paragraphs[paragraph_at(map, scalar, 1)]; local = scalar - e->scalar;
    position = lower_bound(e->body->grapheme_scalars, e->body->graphemes + 1, local);
    return position <= e->body->graphemes && e->body->grapheme_scalars[position] == local;
}
int XmTextUnicodePreviousGrapheme(const XmTextUnicodeMap *map, size_t scalar, size_t *previous) {
    const ParagraphEntry *e; size_t position, local;
    if (!map || !previous || scalar > map->scalars) return 0;
    e = &map->paragraphs[paragraph_at(map, scalar, 1)]; local = scalar - e->scalar;
    position = lower_bound(e->body->grapheme_scalars, e->body->graphemes + 1, local);
    if (position) *previous = e->scalar + e->body->grapheme_scalars[position - 1];
    else if (e != map->paragraphs) {
        const ParagraphEntry *before = e - 1;
        *previous = before->scalar + before->body->grapheme_scalars[before->body->graphemes - 1];
    } else *previous = 0;
    return 1;
}
int XmTextUnicodeNextGrapheme(const XmTextUnicodeMap *map, size_t scalar, size_t *next) {
    const ParagraphEntry *e; size_t position, local;
    if (!map || !next || scalar > map->scalars) return 0;
    e = &map->paragraphs[paragraph_at(map, scalar, 1)]; local = scalar - e->scalar;
    position = lower_bound(e->body->grapheme_scalars, e->body->graphemes + 1, local);
    if (position < e->body->graphemes && e->body->grapheme_scalars[position] == local) ++position;
    *next = e->scalar + e->body->grapheme_scalars[position]; return 1;
}
