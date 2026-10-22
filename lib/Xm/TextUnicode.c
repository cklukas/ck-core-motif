/* SPDX-License-Identifier: LGPL-2.0-or-later */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "TextUnicode.h"
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#ifdef HAVE_UNICODE_TEXT
#include <unicode/ubrk.h>
#include <unicode/utext.h>
#include <unicode/utf8.h>
#endif
struct _XmTextUnicodeMap {
    size_t bytes, scalars, graphemes;
    size_t *scalar_bytes, *grapheme_scalars;
};
int XmTextUnicodeAvailable(void) {
#ifdef HAVE_UNICODE_TEXT
    return 1;
#else
    return 0;
#endif
}
void XmTextUnicodeMapFree(XmTextUnicodeMap *map) {
    if (map) { free(map->scalar_bytes); free(map->grapheme_scalars); free(map); }
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
XmTextUnicodeMap *XmTextUnicodeMapCreate(const char *utf8, size_t length) {
#ifdef HAVE_UNICODE_TEXT
    XmTextUnicodeMap *map;
    int32_t offset = 0, boundary;
    UErrorCode error = U_ZERO_ERROR;
    UBreakIterator *iterator;
    UText *text;
    if ((!utf8 && length) || length > INT32_MAX || length > SIZE_MAX / sizeof(size_t) - 1) return NULL;
    map = (XmTextUnicodeMap *)calloc(1, sizeof(*map));
    if (!map) return NULL;
    map->bytes = length;
    map->scalar_bytes = (size_t *)malloc((length + 1) * sizeof(size_t));
    map->grapheme_scalars = (size_t *)malloc((length + 1) * sizeof(size_t));
    if (!map->scalar_bytes || !map->grapheme_scalars) { XmTextUnicodeMapFree(map); return NULL; }
    while ((size_t)offset < length) {
        UChar32 scalar;
        map->scalar_bytes[map->scalars++] = (size_t)offset;
        U8_NEXT(utf8, offset, (int32_t)length, scalar);
        if (scalar <= 0) { XmTextUnicodeMapFree(map); return NULL; }
    }
    map->scalar_bytes[map->scalars] = length;
    text = utext_openUTF8(NULL, utf8 ? utf8 : "", (int64_t)length, &error);
    iterator = ubrk_open(UBRK_CHARACTER, "root", NULL, 0, &error);
    if (U_FAILURE(error) || !text || !iterator) {
        if (text) utext_close(text);
        if (iterator) ubrk_close(iterator);
        XmTextUnicodeMapFree(map); return NULL;
    }
    ubrk_setUText(iterator, text, &error);
    if (U_FAILURE(error)) { utext_close(text); ubrk_close(iterator); XmTextUnicodeMapFree(map); return NULL; }
    for (boundary = ubrk_first(iterator); boundary != UBRK_DONE; boundary = ubrk_next(iterator)) {
        size_t scalar = lower_bound(map->scalar_bytes, map->scalars + 1, (size_t)boundary);
        if (scalar > map->scalars || map->scalar_bytes[scalar] != (size_t)boundary) {
            utext_close(text); ubrk_close(iterator); XmTextUnicodeMapFree(map); return NULL;
        }
        map->grapheme_scalars[map->graphemes++] = scalar;
    }
    /* Both endpoints are stored, including the single endpoint of empty text. */
    if (map->graphemes) --map->graphemes;
    utext_close(text); ubrk_close(iterator);
    return map;
#else
    (void)utf8; (void)length; return NULL;
#endif
}
size_t XmTextUnicodeByteCount(const XmTextUnicodeMap *map) { return map ? map->bytes : 0; }
size_t XmTextUnicodeScalarCount(const XmTextUnicodeMap *map) { return map ? map->scalars : 0; }
size_t XmTextUnicodeGraphemeCount(const XmTextUnicodeMap *map) { return map ? map->graphemes : 0; }
int XmTextUnicodeByteAtScalar(const XmTextUnicodeMap *map, size_t scalar, size_t *byte) {
    if (!map || !byte || scalar > map->scalars) return 0;
    *byte = map->scalar_bytes[scalar]; return 1;
}
int XmTextUnicodeScalarAtByte(const XmTextUnicodeMap *map, size_t byte, size_t *scalar) {
    size_t position;
    if (!map || !scalar || byte > map->bytes) return 0;
    position = lower_bound(map->scalar_bytes, map->scalars + 1, byte);
    if (position > map->scalars || map->scalar_bytes[position] != byte) return 0;
    *scalar = position; return 1;
}
int XmTextUnicodeScalarAtGrapheme(const XmTextUnicodeMap *map, size_t grapheme, size_t *scalar) {
    if (!map || !scalar || grapheme > map->graphemes) return 0;
    *scalar = map->grapheme_scalars[grapheme]; return 1;
}
int XmTextUnicodeIsGraphemeBoundary(const XmTextUnicodeMap *map, size_t scalar) {
    size_t position;
    if (!map || scalar > map->scalars) return 0;
    position = lower_bound(map->grapheme_scalars, map->graphemes + 1, scalar);
    return position <= map->graphemes && map->grapheme_scalars[position] == scalar;
}
int XmTextUnicodePreviousGrapheme(const XmTextUnicodeMap *map, size_t scalar, size_t *previous) {
    size_t position;
    if (!map || !previous || scalar > map->scalars) return 0;
    position = lower_bound(map->grapheme_scalars, map->graphemes + 1, scalar);
    *previous = position ? map->grapheme_scalars[position - 1] : 0; return 1;
}
int XmTextUnicodeNextGrapheme(const XmTextUnicodeMap *map, size_t scalar, size_t *next) {
    size_t position;
    if (!map || !next || scalar > map->scalars) return 0;
    position = lower_bound(map->grapheme_scalars, map->graphemes + 1, scalar);
    if (position < map->graphemes && map->grapheme_scalars[position] == scalar) ++position;
    *next = map->grapheme_scalars[position]; return 1;
}
