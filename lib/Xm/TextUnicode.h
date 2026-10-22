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
XmTextUnicodeMap *XmTextUnicodeMapCreate(const char *utf8, size_t length);
void XmTextUnicodeMapFree(XmTextUnicodeMap *map);
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
