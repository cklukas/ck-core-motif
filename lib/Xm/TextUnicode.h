/* SPDX-License-Identifier: LGPL-2.0-or-later */
#ifndef _XmTextUnicode_h
#define _XmTextUnicode_h
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/* UTF-8 source offsets, Unicode scalar/widget positions in UTF-8 locales and
 * UAX #29 extended grapheme boundaries are distinct coordinate systems.
 * Offsets are exact: a byte inside an encoding or a scalar inside a grapheme
 * is never silently rounded. The immutable map owns its input metadata.
 * Embedded NUL is refused because Motif text widgets use terminated strings.
 * Maximum source length is INT32_MAX bytes (the ICU break iterator limit). */
typedef struct _XmTextUnicodeMap XmTextUnicodeMap;
int XmTextUnicodeAvailable(void);
/* Native widget scalar positions follow Unicode only in a UTF-8 locale.
 * Explicit UTF-8 map construction remains independent of the locale. */
int XmTextUnicodeLocaleAvailable(void);
XmTextUnicodeMap *XmTextUnicodeMapCreate(const char *utf8, size_t length);
/* Each successful Retain owns one reference; NULL reports reference overflow.
 * Maps and paragraph bodies are immutable, with ownership confined to the
 * owning thread. Replace consumes neither its input map nor its UTF-8. */
XmTextUnicodeMap *XmTextUnicodeMapRetain(XmTextUnicodeMap *map);
void XmTextUnicodeMapFree(XmTextUnicodeMap *map);
typedef struct {
    size_t mapped_bytes, mapped_scalars;
    size_t rebuilt_paragraphs, reused_paragraphs;
} XmTextUnicodeMapUsage;
/* Replace [first_scalar,last_scalar), returning a new owned map. Invalid UTF-8,
 * embedded NUL, invalid ranges, size/refcount overflow or allocation failure
 * return NULL without changing the original map. Whole affected LF-delimited
 * hard paragraphs (including the end paragraph) are reanalyzed; CRLF stays in
 * one body. Other bodies are shared. A single hard paragraph can require full
 * reanalysis for Unicode correctness. Prefix metadata costs O(paragraph count),
 * independent of the number of reused scalars. Usage records this map's
 * construction work, not retained memory or later lookups. */
XmTextUnicodeMap *XmTextUnicodeMapReplace(const XmTextUnicodeMap *map,
    size_t first_scalar, size_t last_scalar, const char *utf8, size_t bytes);
int XmTextUnicodeMapGetUsage(const XmTextUnicodeMap *map,
                            XmTextUnicodeMapUsage *usage);
size_t XmTextUnicodeByteCount(const XmTextUnicodeMap *map);
size_t XmTextUnicodeScalarCount(const XmTextUnicodeMap *map);
size_t XmTextUnicodeGraphemeCount(const XmTextUnicodeMap *map);
int XmTextUnicodeByteAtScalar(const XmTextUnicodeMap *map, size_t scalar, size_t *byte);
int XmTextUnicodeScalarAtByte(const XmTextUnicodeMap *map, size_t byte, size_t *scalar);
int XmTextUnicodeScalarAtGrapheme(const XmTextUnicodeMap *map, size_t grapheme, size_t *scalar);
int XmTextUnicodeIsGraphemeBoundary(const XmTextUnicodeMap *map, size_t scalar);
int XmTextUnicodePreviousGrapheme(const XmTextUnicodeMap *map, size_t scalar, size_t *previous);
int XmTextUnicodeNextGrapheme(const XmTextUnicodeMap *map, size_t scalar, size_t *next);
#ifdef __cplusplus
}
#endif
#endif
