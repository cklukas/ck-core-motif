/* SPDX-License-Identifier: LGPL-2.0-or-later */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "TextLayout.h"
#include "TextUnicode.h"
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNICODE_TEXT
#include <unicode/ubidi.h>
#include <unicode/ubrk.h>
#include <unicode/utf16.h>
#include <hb-ft.h>
#include <hb-ot.h>
#include <unicode/uchar.h>
#include <unicode/utf8.h>
#endif
struct LayoutFont { XftFont *font; FT_Face face; int owned; };
struct LayoutGlyph { XftFont *font; unsigned int glyph; double x, y; size_t cluster; int visible; };
struct LayoutCaret { size_t scalar; int affinity; double x; };
struct _XmTextParagraph {
    unsigned int references;
    Display *display;
    XmTextUnicodeMap *map;
    struct LayoutFont *fonts;
    size_t font_count;
    char *utf8;
    uint32_t *scalars, *scripts;
    size_t *font_at;
    int32_t *utf16_at;
    unsigned char *line_breaks;
    double *advance_prefix;
    size_t scalar_count, byte_count;
    int ascent, descent;
    double tab_width;
#ifdef HAVE_UNICODE_TEXT
    UChar *utf16;
    UBiDi *bidi;
#endif
};
struct ShapeGlyph {
    unsigned int index;
    int32_t x_advance, y_advance, x_offset, y_offset;
    FT_Face ftface;
    size_t cluster;
    int rtl;
};
struct _XmTextLayout {
    XmTextParagraph *paragraph;
    Display *display;
    XmTextUnicodeMap *map;
    struct LayoutGlyph *glyphs;
    size_t glyph_count;
    struct LayoutCaret *carets;
    size_t caret_count;
    double *leading, *trailing;
    double width;
    int ascent, descent;
    int rtl;
};
int XmTextLayoutAvailable(void) { return XmTextUnicodeAvailable(); }
XmTextParagraph *XmTextParagraphReference(XmTextParagraph *paragraph) {
    if (!paragraph || paragraph->references == UINT_MAX) return NULL;
    ++paragraph->references; return paragraph;
}
void XmTextParagraphFree(XmTextParagraph *paragraph) {
    size_t i;
    if (!paragraph || --paragraph->references) return;
    for (i = 0; i < paragraph->font_count; ++i) {
        XftUnlockFace(paragraph->fonts[i].font);
        if (paragraph->fonts[i].owned) XftFontClose(paragraph->display, paragraph->fonts[i].font);
    }
#ifdef HAVE_UNICODE_TEXT
    if (paragraph->bidi) ubidi_close(paragraph->bidi);
    free(paragraph->utf16);
#endif
    XmTextUnicodeMapFree(paragraph->map); free(paragraph->fonts); free(paragraph->utf8);
    free(paragraph->line_breaks); free(paragraph->advance_prefix);
    free(paragraph->scalars); free(paragraph->scripts); free(paragraph->font_at); free(paragraph->utf16_at); free(paragraph);
}
size_t XmTextParagraphScalarCount(const XmTextParagraph *paragraph) { return paragraph ? paragraph->scalar_count : 0; }
void XmTextLayoutFree(XmTextLayout *layout) {
    if (!layout) return;
    XmTextUnicodeMapFree(layout->map);
    free(layout->glyphs); free(layout->carets);
    free(layout->leading); free(layout->trailing);
    XmTextParagraphFree(layout->paragraph); free(layout);
}
#ifdef HAVE_UNICODE_TEXT
static int font_covers(XftFont *font, const char *text, size_t first, size_t last) {
    int32_t index = (int32_t)first;
    while ((size_t)index < last) {
        UChar32 scalar;
        U8_NEXT(text, index, (int32_t)last, scalar);
        if (scalar < 0) return 0;
        if (scalar != '\t' && !u_hasBinaryProperty(scalar, UCHAR_DEFAULT_IGNORABLE_CODE_POINT) &&
            !FcCharSetHasChar(font->charset, (FcChar32)scalar)) return 0;
    }
    return 1;
}
static size_t font_for_grapheme(XmTextParagraph *layout, XftFont *primary,
                                const char *text, size_t first, size_t last) {
    size_t i;
    FcPattern *pattern, *matched;
    FcCharSet *characters;
    FcResult result;
    XftFont *font;
    int32_t index = (int32_t)first;
    for (i = 0; i < layout->font_count; ++i)
        if (font_covers(layout->fonts[i].font, text, first, last)) return i;
    /* A resolved Xft pattern contains file, PostScript name, foundry and
     * other face identities. Reusing it as a query can outrank charset coverage.
     * Copy only requested style/size/rendering properties into a fresh query. */
    pattern = FcPatternCreate();
    if (pattern) {
        static const char *const properties[] = {
            FC_FAMILY, FC_STYLE, FC_SLANT, FC_WEIGHT, FC_WIDTH, FC_SPACING,
            FC_SIZE, FC_PIXEL_SIZE, FC_DPI, FC_SCALE, FC_MATRIX,
            FC_ANTIALIAS, FC_HINTING, FC_HINT_STYLE, FC_AUTOHINT,
            FC_RGBA, FC_LCD_FILTER, FC_EMBEDDED_BITMAP, FC_EMBOLDEN
        };
        size_t property;
        for (property = 0; property < sizeof(properties) / sizeof(properties[0]); ++property) {
            FcValue value;
            if (FcPatternGet(primary->pattern, properties[property], 0, &value) == FcResultMatch &&
                !FcPatternAdd(pattern, properties[property], value, FcTrue)) {
                FcPatternDestroy(pattern); pattern = NULL; break;
            }
        }
    }
    characters = FcCharSetCreate();
    if (!pattern || !characters) { if (pattern) FcPatternDestroy(pattern); if (characters) FcCharSetDestroy(characters); return SIZE_MAX; }
    while ((size_t)index < last) {
        UChar32 scalar; U8_NEXT(text, index, (int32_t)last, scalar);
        if (scalar != '\t' && !u_hasBinaryProperty(scalar, UCHAR_DEFAULT_IGNORABLE_CODE_POINT)) FcCharSetAddChar(characters, (FcChar32)scalar);
    }
    if (!FcPatternAddCharSet(pattern, FC_CHARSET, characters)) {
        FcCharSetDestroy(characters); FcPatternDestroy(pattern); return SIZE_MAX;
    }
    FcCharSetDestroy(characters);
    matched = XftFontMatch(layout->display, DefaultScreen(layout->display), pattern, &result);
    FcPatternDestroy(pattern);
    if (!matched) return SIZE_MAX;
    font = XftFontOpenPattern(layout->display, matched);
    if (!font) { FcPatternDestroy(matched); return SIZE_MAX; }
    if (!font_covers(font, text, first, last)) { XftFontClose(layout->display, font); return SIZE_MAX; }
    layout->fonts[layout->font_count].font = font;
    layout->fonts[layout->font_count].face = XftLockFace(font);
    layout->fonts[layout->font_count].owned = 1;
    if (!layout->fonts[layout->font_count].face) { XftFontClose(layout->display, font); return SIZE_MAX; }
    if (font->ascent > layout->ascent) layout->ascent = font->ascent;
    if (font->descent > layout->descent) layout->descent = font->descent;
    return layout->font_count++;
}
static int compare_offset(const void *a, const void *b) {
    size_t left = *(const size_t *)a, right = *(const size_t *)b;
    return left < right ? -1 : left > right ? 1 : 0;
}
static int compare_caret(const void *a, const void *b) {
    const struct LayoutCaret *left = (const struct LayoutCaret *)a, *right = (const struct LayoutCaret *)b;
    if (left->x < right->x) return -1;
    if (left->x > right->x) return 1;
    if (left->scalar < right->scalar) return -1;
    if (left->scalar > right->scalar) return 1;
    return left->affinity - right->affinity;
}
#endif
#ifdef HAVE_UNICODE_TEXT
struct ScriptBracket { UChar32 closing; hb_script_t script; };
static int resolve_scripts(XmTextParagraph *paragraph) {
    struct ScriptBracket *stack;
    size_t i, depth = 0;
    hb_script_t current = HB_SCRIPT_INVALID;
    hb_unicode_funcs_t *unicode = hb_unicode_funcs_get_default();
    stack = (struct ScriptBracket *)malloc((paragraph->scalar_count + 1) * sizeof(*stack));
    if (!stack) return 0;
    for (i = 0; i < paragraph->scalar_count; ++i) {
        UChar32 scalar = (UChar32)paragraph->scalars[i];
        hb_script_t script = hb_unicode_script(unicode, (hb_codepoint_t)scalar);
        int bracket = u_getIntPropertyValue(scalar, UCHAR_BIDI_PAIRED_BRACKET_TYPE);
        if (hb_unicode_general_category(unicode, (hb_codepoint_t)scalar) == HB_UNICODE_GENERAL_CATEGORY_NON_SPACING_MARK)
            script = HB_SCRIPT_INHERITED;
        if (script != HB_SCRIPT_COMMON && script != HB_SCRIPT_INHERITED && script != HB_SCRIPT_UNKNOWN) {
            size_t pending;
            current = script;
            for (pending = 0; pending < depth; ++pending) if (stack[pending].script == HB_SCRIPT_INVALID) stack[pending].script = current;
        } else if (bracket == U_BPT_OPEN) {
            stack[depth++] = (struct ScriptBracket){u_getBidiPairedBracket(scalar), current};
            if (current != HB_SCRIPT_INVALID) script = current;
        } else if (bracket == U_BPT_CLOSE) {
            while (depth && stack[depth - 1].closing != scalar) --depth;
            if (depth) {
                if (stack[depth - 1].script != HB_SCRIPT_INVALID) script = stack[depth - 1].script;
                --depth;
            } else if (current != HB_SCRIPT_INVALID) script = current;
        } else if (current != HB_SCRIPT_INVALID) script = current;
        paragraph->scripts[i] = (uint32_t)script;
    }
    current = HB_SCRIPT_COMMON;
    for (i = paragraph->scalar_count; i > 0; --i) {
        hb_script_t script = (hb_script_t)paragraph->scripts[i - 1];
        if (script == HB_SCRIPT_COMMON || script == HB_SCRIPT_INHERITED || script == HB_SCRIPT_UNKNOWN)
            paragraph->scripts[i - 1] = (uint32_t)current;
        else current = script;
    }
    free(stack); return 1;
}
static size_t scalar_at_utf16(const XmTextParagraph *paragraph, int32_t offset) {
    size_t lower = 0, upper = paragraph->scalar_count + 1;
    while (lower < upper) {
        size_t middle = lower + (upper - lower) / 2;
        if (paragraph->utf16_at[middle] < offset) lower = middle + 1; else upper = middle;
    }
    return lower;
}
static int append_item(const XmTextParagraph *paragraph, size_t first, size_t last,
                       size_t line_first, size_t line_last, int rtl,
                       struct ShapeGlyph **glyphs, size_t *count, size_t *capacity) {
    hb_buffer_t *buffer = hb_buffer_create();
    hb_font_t *font;
    hb_glyph_info_t *info;
    hb_glyph_position_t *position;
    unsigned int length, i;
    hb_buffer_flags_t flags = HB_BUFFER_FLAG_DEFAULT;
    size_t font_index = paragraph->font_at[first], byte_base;
    if (!buffer || !hb_buffer_allocation_successful(buffer)) { if (buffer) hb_buffer_destroy(buffer); return 0; }
    font = hb_ft_font_create_referenced(paragraph->fonts[font_index].face);
    if (!font) { hb_buffer_destroy(buffer); return 0; }
    if (first == line_first) flags = (hb_buffer_flags_t)(flags | HB_BUFFER_FLAG_BOT);
    if (last == line_last) flags = (hb_buffer_flags_t)(flags | HB_BUFFER_FLAG_EOT);
    hb_buffer_set_flags(buffer, flags);
    hb_buffer_set_direction(buffer, rtl ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer, (hb_script_t)paragraph->scripts[first]);
    hb_buffer_set_language(buffer, hb_language_get_default());
    hb_buffer_set_cluster_level(buffer, HB_BUFFER_CLUSTER_LEVEL_MONOTONE_GRAPHEMES);
    /* Bidi levels come from the complete paragraph. Shaping context spans the
     * complete line (including neighboring font/script items), but excludes
     * letters beyond its actual break: joining must restart at line edges. */
    hb_buffer_add_utf32(buffer, paragraph->scalars + line_first, (int)(line_last - line_first),
                        (unsigned int)(first - line_first), (int)(last - first));
    hb_shape(font, buffer, NULL, 0);
    if (!hb_buffer_allocation_successful(buffer)) { hb_font_destroy(font); hb_buffer_destroy(buffer); return 0; }
    info = hb_buffer_get_glyph_infos(buffer, &length);
    position = hb_buffer_get_glyph_positions(buffer, NULL);
    if ((size_t)length > SIZE_MAX - *count || *count + length > SIZE_MAX / sizeof(struct ShapeGlyph)) {
        hb_font_destroy(font); hb_buffer_destroy(buffer); return 0;
    }
    if (*count + length > *capacity) {
        size_t required = *count + length;
        struct ShapeGlyph *grown;
        if (required < SIZE_MAX / 2) required *= 2;
        if (required > SIZE_MAX / sizeof(struct ShapeGlyph)) required = *count + length;
        grown = (struct ShapeGlyph *)realloc(*glyphs, required * sizeof(struct ShapeGlyph));
        if (!grown) { hb_font_destroy(font); hb_buffer_destroy(buffer); return 0; }
        *glyphs = grown; *capacity = required;
    }
    XmTextUnicodeByteAtScalar(paragraph->map, line_first, &byte_base);
    for (i = 0; i < length; ++i) {
        size_t byte;
        size_t scalar_cluster = line_first + info[i].cluster;
        if (scalar_cluster < first || scalar_cluster >= last ||
            !XmTextUnicodeByteAtScalar(paragraph->map, scalar_cluster, &byte)) {
            hb_font_destroy(font); hb_buffer_destroy(buffer); return 0;
        }
        (*glyphs)[(*count)++] = (struct ShapeGlyph){info[i].codepoint,
            position[i].x_advance, position[i].y_advance, position[i].x_offset, position[i].y_offset,
            paragraph->fonts[font_index].face, byte - byte_base, rtl};
    }
    hb_font_destroy(font); hb_buffer_destroy(buffer); return 1;
}
static int shape_line(const XmTextParagraph *paragraph, size_t first, size_t last,
                      struct ShapeGlyph **glyphs, size_t *count) {
    struct Item { size_t first, last; };
    struct Item *items;
    UBiDi *line;
    UErrorCode error = U_ZERO_ERROR;
    int32_t runs, run;
    size_t capacity = 0;
    *glyphs = NULL; *count = 0;
    if (first == last) return 1;
    line = ubidi_openSized(paragraph->utf16_at[last] - paragraph->utf16_at[first], 0, &error);
    items = (struct Item *)malloc((last - first) * sizeof(*items));
    if (!line || !items || U_FAILURE(error)) { if (line) ubidi_close(line); free(items); return 0; }
    ubidi_setLine(paragraph->bidi, paragraph->utf16_at[first], paragraph->utf16_at[last], line, &error);
    runs = ubidi_countRuns(line, &error);
    if (U_FAILURE(error)) { ubidi_close(line); free(items); return 0; }
    for (run = 0; run < runs; ++run) {
        int32_t logical, length;
        UBiDiDirection direction = ubidi_getVisualRun(line, run, &logical, &length);
        size_t begin = scalar_at_utf16(paragraph, paragraph->utf16_at[first] + logical);
        size_t end = scalar_at_utf16(paragraph, paragraph->utf16_at[first] + logical + length);
        size_t at = begin, item_count = 0, i;
        if (begin > end || end > last || begin < first || paragraph->utf16_at[begin] != paragraph->utf16_at[first] + logical ||
            paragraph->utf16_at[end] != paragraph->utf16_at[first] + logical + length) goto fail;
        while (at < end) {
            size_t next = at + 1;
            while (next < end && paragraph->font_at[next] == paragraph->font_at[at] && paragraph->scripts[next] == paragraph->scripts[at]) ++next;
            items[item_count++] = (struct Item){at, next}; at = next;
        }
        for (i = 0; i < item_count; ++i) {
            const struct Item *item = &items[direction == UBIDI_RTL ? item_count - i - 1 : i];
            if (!append_item(paragraph, item->first, item->last, first, last, direction == UBIDI_RTL, glyphs, count, &capacity)) goto fail;
        }
    }
    ubidi_close(line); free(items); return 1;
fail:
    ubidi_close(line); free(items); free(*glyphs); *glyphs = NULL; *count = 0; return 0;
}
#endif
XmTextParagraph *XmTextParagraphCreate(Display *display, XftFont *primary, const char *text, size_t length, double tab_width) {
#ifdef HAVE_UNICODE_TEXT
    XmTextParagraph *paragraph;
    size_t i, graphemes;
    int32_t offset = 0, utf16_length = 0;
    UErrorCode error = U_ZERO_ERROR;
    if (!display || !primary || (!text && length) || length > INT32_MAX ||
        !isfinite(tab_width) || tab_width < 1.0 / 64.0 || tab_width > INT_MAX / 64.0 ||
        (length && (memchr(text, '\n', length) || memchr(text, '\r', length)))) return NULL;
    paragraph = (XmTextParagraph *)calloc(1, sizeof(*paragraph));
    if (!paragraph) return NULL;
    paragraph->references = 1; paragraph->display = display; paragraph->tab_width = tab_width;
    paragraph->ascent = primary->ascent; paragraph->descent = primary->descent; paragraph->byte_count = length;
    paragraph->map = XmTextUnicodeMapCreate(text, length);
    if (!paragraph->map) goto fail;
    paragraph->scalar_count = XmTextUnicodeScalarCount(paragraph->map);
    graphemes = XmTextUnicodeGraphemeCount(paragraph->map);
    if (paragraph->scalar_count > INT32_MAX / 2 || paragraph->scalar_count > SIZE_MAX / (2 * sizeof(UChar)) - 1 ||
        paragraph->scalar_count > SIZE_MAX / sizeof(size_t) - 1 || graphemes > SIZE_MAX / sizeof(struct LayoutFont) - 1) goto fail;
    paragraph->utf8 = (char *)malloc(length + 1);
    paragraph->scalars = (uint32_t *)malloc((paragraph->scalar_count + 1) * sizeof(uint32_t));
    paragraph->scripts = (uint32_t *)malloc((paragraph->scalar_count + 1) * sizeof(uint32_t));
    paragraph->font_at = (size_t *)malloc((paragraph->scalar_count + 1) * sizeof(size_t));
    paragraph->utf16_at = (int32_t *)malloc((paragraph->scalar_count + 1) * sizeof(int32_t));
    paragraph->utf16 = (UChar *)malloc((2 * paragraph->scalar_count + 1) * sizeof(UChar));
    paragraph->fonts = (struct LayoutFont *)calloc(graphemes + 1, sizeof(struct LayoutFont));
    if (!paragraph->utf8 || !paragraph->scalars || !paragraph->scripts || !paragraph->font_at || !paragraph->utf16_at || !paragraph->utf16 || !paragraph->fonts) goto fail;
    if (length) memcpy(paragraph->utf8, text, length);
    paragraph->utf8[length] = '\0';
    for (i = 0; i < paragraph->scalar_count; ++i) {
        UChar32 scalar;
        U8_NEXT(text, offset, (int32_t)length, scalar);
        paragraph->scalars[i] = (uint32_t)scalar; paragraph->utf16_at[i] = utf16_length;
        U16_APPEND_UNSAFE(paragraph->utf16, utf16_length, scalar);
    }
    paragraph->utf16_at[paragraph->scalar_count] = utf16_length; paragraph->utf16[utf16_length] = 0;
    paragraph->bidi = ubidi_openSized(utf16_length, 0, &error);
    if (!paragraph->bidi || U_FAILURE(error)) goto fail;
    ubidi_setPara(paragraph->bidi, paragraph->utf16, utf16_length, UBIDI_DEFAULT_LTR, NULL, &error);
    if (U_FAILURE(error) || !resolve_scripts(paragraph)) goto fail;
    paragraph->fonts[0].font = primary; paragraph->fonts[0].face = XftLockFace(primary);
    if (!paragraph->fonts[0].face) goto fail;
    paragraph->font_count = 1;
    for (i = 0; i < graphemes; ++i) {
        size_t first, last, byte_first, byte_last, font, scalar;
        XmTextUnicodeScalarAtGrapheme(paragraph->map, i, &first); XmTextUnicodeScalarAtGrapheme(paragraph->map, i + 1, &last);
        XmTextUnicodeByteAtScalar(paragraph->map, first, &byte_first); XmTextUnicodeByteAtScalar(paragraph->map, last, &byte_last);
        font = font_for_grapheme(paragraph, primary, text, byte_first, byte_last);
        if (font == SIZE_MAX) goto fail;
        for (scalar = first; scalar < last; ++scalar) paragraph->font_at[scalar] = font;
    }
    {
        UBreakIterator *breaks;
        int32_t boundary;
        paragraph->line_breaks = (unsigned char *)calloc(paragraph->scalar_count + 1, 1);
        if (!paragraph->line_breaks) goto fail;
        breaks = ubrk_open(UBRK_LINE, "root", paragraph->utf16, utf16_length, &error);
        if (!breaks || U_FAILURE(error)) { if (breaks) ubrk_close(breaks); goto fail; }
        for (boundary = ubrk_first(breaks); boundary != UBRK_DONE; boundary = ubrk_next(breaks)) {
            size_t scalar = scalar_at_utf16(paragraph, boundary);
            if (scalar <= paragraph->scalar_count && paragraph->utf16_at[scalar] == boundary &&
                XmTextUnicodeIsGraphemeBoundary(paragraph->map, scalar)) paragraph->line_breaks[scalar] = 1;
        }
        ubrk_close(breaks);
    }
    return paragraph;
fail:
    XmTextParagraphFree(paragraph); return NULL;
#else
    (void)display; (void)primary; (void)text; (void)length; (void)tab_width; return NULL;
#endif
}
XmTextLayout *XmTextLayoutCreate(Display *display, XftFont *primary, const char *text, size_t length) {
    XGlyphInfo space;
    if (!display || !primary) return NULL;
    XftTextExtentsUtf8(display, primary, (const FcChar8 *)" ", 1, &space);
    return XmTextLayoutCreateWithTabWidth(display, primary, text, length, 8.0 * (space.xOff > 0 ? space.xOff : 1));
}
XmTextLayout *XmTextLayoutCreateWithTabWidth(Display *display, XftFont *primary, const char *text, size_t length, double tab_width) {
    XmTextParagraph *paragraph = XmTextParagraphCreate(display, primary, text, length, tab_width);
    XmTextLayout *layout;
    if (!paragraph) return NULL;
    layout = XmTextParagraphLine(paragraph, 0, paragraph->scalar_count);
    XmTextParagraphFree(paragraph); return layout;
}
XmTextLayout *XmTextParagraphLine(XmTextParagraph *paragraph, size_t first, size_t last) {
#ifdef HAVE_UNICODE_TEXT
    XmTextLayout *layout = NULL;
    struct ShapeGlyph *glyphs = NULL;
    size_t count = 0, i, scalars, first_byte, last_byte, length;
    size_t *clusters = NULL, cluster_count = 0;
    int64_t pen = 0;
    const char *text;
    if (!paragraph || first > last || last > paragraph->scalar_count ||
        !XmTextUnicodeIsGraphemeBoundary(paragraph->map, first) || !XmTextUnicodeIsGraphemeBoundary(paragraph->map, last)) return NULL;
    XmTextUnicodeByteAtScalar(paragraph->map, first, &first_byte); XmTextUnicodeByteAtScalar(paragraph->map, last, &last_byte);
    text = paragraph->utf8 + first_byte; length = last_byte - first_byte; scalars = last - first;
    layout = (XmTextLayout *)calloc(1, sizeof(*layout));
    if (!layout) return NULL;
    layout->paragraph = XmTextParagraphReference(paragraph); layout->display = paragraph->display;
    layout->ascent = paragraph->ascent; layout->descent = paragraph->descent;
    layout->map = XmTextUnicodeMapCreate(text, length);
    if (!layout->paragraph || !layout->map || scalars > SIZE_MAX / (2 * sizeof(struct LayoutCaret)) - 1) goto fail;
    layout->leading = (double *)malloc((scalars + 1) * sizeof(double));
    layout->trailing = (double *)malloc((scalars + 1) * sizeof(double));
    layout->carets = (struct LayoutCaret *)malloc(2 * (scalars + 1) * sizeof(struct LayoutCaret));
    if (!layout->leading || !layout->trailing || !layout->carets) goto fail;
    for (i = 0; i <= scalars; ++i) layout->leading[i] = layout->trailing[i] = NAN;
    if (!scalars) {
        layout->leading[0] = layout->trailing[0] = 0;
        layout->carets[0] = (struct LayoutCaret){0, 0, 0}; layout->caret_count = 1; return layout;
    }
    if (!shape_line(paragraph, first, last, &glyphs, &count) || !count || count > SIZE_MAX / sizeof(struct LayoutGlyph) || count > SIZE_MAX / sizeof(size_t) - 1) goto fail;
    layout->glyphs = (struct LayoutGlyph *)calloc(count, sizeof(struct LayoutGlyph));
    clusters = (size_t *)malloc((count + 1) * sizeof(size_t));
    if (!layout->glyphs || !clusters) goto fail;
    layout->glyph_count = count;
    for (i = 0; i < count; ++i) clusters[i] = glyphs[i].cluster;
    clusters[count] = length; qsort(clusters, count + 1, sizeof(size_t), compare_offset);
    for (i = 0; i <= count; ++i) if (!cluster_count || clusters[i] != clusters[cluster_count - 1]) clusters[cluster_count++] = clusters[i];
    layout->rtl = (ubidi_getParaLevel(paragraph->bidi) & 1) != 0;
    {
        int64_t distance = 0;
        const int64_t tab = (int64_t)lround(paragraph->tab_width * 64.0);
        for (i = 0; i < count; ++i) {
            size_t at = layout->rtl ? count - i - 1 : i;
            if (glyphs[at].cluster < length && text[glyphs[at].cluster] == '\t') {
                glyphs[at].x_advance = (int)(tab - distance % tab); glyphs[at].x_offset = 0; glyphs[at].y_offset = 0;
            }
            distance += glyphs[at].x_advance;
            if (distance < 0 || distance > INT_MAX) goto fail;
        }
    }
    for (i = 0; i < count; ++i) {
        size_t font;
        for (font = 0; font < paragraph->font_count && paragraph->fonts[font].face != glyphs[i].ftface; ++font) {}
        if (font == paragraph->font_count) goto fail;
        layout->glyphs[i] = (struct LayoutGlyph){paragraph->fonts[font].font, glyphs[i].index,
            (double)(pen + glyphs[i].x_offset) / 64.0, -(double)glyphs[i].y_offset / 64.0,
            glyphs[i].cluster, !(glyphs[i].cluster < length && text[glyphs[i].cluster] == '\t')};
        pen += glyphs[i].x_advance;
        if (pen < 0 || pen > INT_MAX) goto fail;
    }
    layout->width = (double)pen / 64.0;
    /* Clusters share a single visual interval. ICU supplies the complete UAX29
     * boundaries; GDEF supplies internal ligature carets where available. Fonts
     * without GDEF use evenly spaced insertion positions within that ligature. */
    pen = 0;
    for (i = 0; i < count;) {
        size_t next = i + 1, cluster = glyphs[i].cluster, end_byte = length, first_scalar, last_scalar, j, stops = 0;
        int64_t advance = glyphs[i].x_advance;
        double left = (double)pen / 64.0, right;
        int rtl;
        while (next < count && glyphs[next].cluster == cluster) advance += glyphs[next++].x_advance;
        {
            size_t lower = 0, upper = cluster_count;
            while (lower < upper) { size_t middle = lower + (upper - lower) / 2; if (clusters[middle] <= cluster) lower = middle + 1; else upper = middle; }
            if (lower >= cluster_count) goto fail;
            end_byte = clusters[lower];
        }
        if (!XmTextUnicodeScalarAtByte(layout->map, cluster, &first_scalar) || !XmTextUnicodeScalarAtByte(layout->map, end_byte, &last_scalar)) goto fail;
        right = (double)(pen + advance) / 64.0;
        rtl = glyphs[i].rtl;
        for (j = first_scalar; j <= last_scalar; ++j) if (XmTextUnicodeIsGraphemeBoundary(layout->map, j)) ++stops;
        if (stops < 2) goto fail;
        {
            size_t ordinal = 0;
            hb_font_t *font = NULL;
            hb_position_t *gdef = NULL;
            unsigned int gdef_count = (unsigned int)(stops - 2);
            if (next == i + 1 && gdef_count) {
                font = hb_ft_font_create_referenced(glyphs[i].ftface);
                gdef = (hb_position_t *)malloc(gdef_count * sizeof(hb_position_t));
                if (!font || !gdef) { if (font) hb_font_destroy(font); free(gdef); goto fail; }
                hb_ot_layout_get_ligature_carets(font, rtl ? HB_DIRECTION_RTL : HB_DIRECTION_LTR, glyphs[i].index, 0, &gdef_count, gdef);
            }
            for (j = first_scalar; j <= last_scalar; ++j) if (XmTextUnicodeIsGraphemeBoundary(layout->map, j)) {
                double fraction = (double)ordinal / (double)(stops - 1);
                double x = rtl ? right - (right - left) * fraction : left + (right - left) * fraction;
                if (ordinal && ordinal + 1 < stops && gdef && gdef_count >= stops - 2) {
                    size_t caret_index = rtl ? stops - 2 - ordinal : ordinal - 1;
                    x = layout->glyphs[i].x + (double)gdef[caret_index] / 64.0;
                }
                if (j == first_scalar) layout->leading[j] = x;
                else if (j == last_scalar) layout->trailing[j] = x;
                else layout->leading[j] = layout->trailing[j] = x;
                ++ordinal;
            }
            if (font) hb_font_destroy(font);
            free(gdef);
        }
        pen += advance; i = next;
    }
    for (i = 0; i <= scalars; ++i) if (XmTextUnicodeIsGraphemeBoundary(layout->map, i)) {
        double leading = layout->leading[i], trailing = layout->trailing[i];
        if (isnan(leading)) leading = trailing;
        if (isnan(trailing)) trailing = leading;
        if (isnan(leading) || isnan(trailing)) goto fail;
        layout->leading[i] = leading; layout->trailing[i] = trailing;
        layout->carets[layout->caret_count++] = (struct LayoutCaret){i, 0, leading};
        if (fabs(leading - trailing) > 0.01) layout->carets[layout->caret_count++] = (struct LayoutCaret){i, 1, trailing};
    }
    qsort(layout->carets, layout->caret_count, sizeof(struct LayoutCaret), compare_caret);
    free(glyphs); free(clusters); return layout;
fail:
    free(glyphs); free(clusters); XmTextLayoutFree(layout); return NULL;
#else
    (void)paragraph; (void)first; (void)last; return NULL;
#endif
}
#ifdef HAVE_UNICODE_TEXT
static int paragraph_advances(XmTextParagraph *paragraph) {
    XmTextLayout *whole;
    size_t first = 0, last, i;
    double total = 0;
    if (paragraph->advance_prefix) return 1;
    whole = XmTextParagraphLine(paragraph, 0, paragraph->scalar_count);
    if (!whole) return 0;
    paragraph->advance_prefix = (double *)malloc((paragraph->scalar_count + 1) * sizeof(double));
    if (!paragraph->advance_prefix) { XmTextLayoutFree(whole); return 0; }
    paragraph->advance_prefix[0] = 0;
    while (first < paragraph->scalar_count) {
        if (!XmTextUnicodeNextGrapheme(paragraph->map, first, &last)) {
            free(paragraph->advance_prefix); paragraph->advance_prefix = NULL;
            XmTextLayoutFree(whole); return 0;
        }
        total += fabs(whole->leading[first] - whole->trailing[last]);
        for (i = first + 1; i <= last; ++i) paragraph->advance_prefix[i] = total;
        first = last;
    }
    XmTextLayoutFree(whole); return 1;
}
static size_t previous_line_boundary(const XmTextParagraph *paragraph, size_t at, size_t first, int legal) {
    size_t previous;
    while (at > first && XmTextUnicodePreviousGrapheme(paragraph->map, at, &previous)) {
        at = previous;
        if (!legal || paragraph->line_breaks[at]) return at;
    }
    return first;
}
static size_t next_line_boundary(const XmTextParagraph *paragraph, size_t at, int legal) {
    size_t next;
    while (at < paragraph->scalar_count && XmTextUnicodeNextGrapheme(paragraph->map, at, &next)) {
        at = next;
        if (!legal || paragraph->line_breaks[at]) return at;
    }
    return paragraph->scalar_count;
}
#endif
XmTextLayout *XmTextParagraphWrapLine(XmTextParagraph *paragraph, size_t first, double width, size_t *last) {
#ifdef HAVE_UNICODE_TEXT
    size_t lower, upper, candidate, estimated, minimum;
    int emergency = 0;
    XmTextLayout *line;
    if (!paragraph || !last || !isfinite(width) || width <= 0 || first > paragraph->scalar_count ||
        !XmTextUnicodeIsGraphemeBoundary(paragraph->map, first)) return NULL;
    if (first == paragraph->scalar_count) { *last = first; return XmTextParagraphLine(paragraph, first, first); }
    if (!paragraph_advances(paragraph)) return NULL;
    /* The paragraph capture supplies a monotone advance index. Locate a close
     * candidate without repeatedly shaping a growing, potentially huge word.
     * Actual line-edge shaping below verifies and adjusts the chosen break. */
    lower = first; upper = paragraph->scalar_count + 1;
    while (lower < upper) {
        size_t middle = lower + (upper - lower) / 2;
        if (paragraph->advance_prefix[middle] - paragraph->advance_prefix[first] <= width) lower = middle + 1;
        else upper = middle;
    }
    estimated = lower > first ? lower - 1 : first;
    if (!XmTextUnicodeIsGraphemeBoundary(paragraph->map, estimated))
        XmTextUnicodePreviousGrapheme(paragraph->map, estimated, &estimated);
    candidate = estimated;
    while (candidate > first && !paragraph->line_breaks[candidate])
        candidate = previous_line_boundary(paragraph, candidate, first, 1);
    minimum = next_line_boundary(paragraph, first, 0);
    if (candidate == first) { emergency = 1; candidate = estimated > first ? estimated : minimum; }
    for (;;) {
        line = XmTextParagraphLine(paragraph, first, candidate);
        if (!line) return NULL;
        if (line->width <= width || candidate == minimum) break;
        XmTextLayoutFree(line);
        lower = previous_line_boundary(paragraph, candidate, first, !emergency);
        if (lower == first) {
            emergency = 1;
            lower = previous_line_boundary(paragraph, candidate, first, 0);
        }
        candidate = lower > first ? lower : minimum;
    }
    /* A line boundary can remove joining/kerning and change the estimate.
     * Try the next legal break (or grapheme in an overlong word) against the
     * same real capture. Stop at the first overflowing candidate. */
    while (candidate < paragraph->scalar_count) {
        XmTextLayout *next;
        size_t end = next_line_boundary(paragraph, candidate, !emergency);
        if (end <= candidate) break;
        next = XmTextParagraphLine(paragraph, first, end);
        if (!next) { XmTextLayoutFree(line); return NULL; }
        if (next->width > width) { XmTextLayoutFree(next); break; }
        XmTextLayoutFree(line); line = next; candidate = end;
        if (paragraph->line_breaks[candidate]) emergency = 0;
    }
    *last = candidate; return line;
#else
    (void)paragraph; (void)first; (void)width; (void)last; return NULL;
#endif
}
double XmTextLayoutWidth(const XmTextLayout *layout) { return layout ? layout->width : 0; }
size_t XmTextLayoutScalarCount(const XmTextLayout *layout) { return layout ? XmTextUnicodeScalarCount(layout->map) : 0; }
size_t XmTextLayoutGlyphCount(const XmTextLayout *layout) { return layout ? layout->glyph_count : 0; }
int XmTextLayoutBoundary(const XmTextLayout *layout, size_t scalar, int direction, size_t *boundary) {
    if (!layout || !boundary || scalar > XmTextUnicodeScalarCount(layout->map) || (direction != -1 && direction != 1)) return 0;
    if (XmTextUnicodeIsGraphemeBoundary(layout->map, scalar)) { *boundary = scalar; return 1; }
    return direction < 0 ? XmTextUnicodePreviousGrapheme(layout->map, scalar, boundary) : XmTextUnicodeNextGrapheme(layout->map, scalar, boundary);
}
double XmTextLayoutRangeWidth(const XmTextLayout *layout, size_t first, size_t last) {
    double width = 0;
    if (!layout || first > last || !XmTextUnicodeIsGraphemeBoundary(layout->map, first) || !XmTextUnicodeIsGraphemeBoundary(layout->map, last)) return -1;
    while (first < last) {
        size_t next;
        if (!XmTextUnicodeNextGrapheme(layout->map, first, &next) || next <= first || next > last) return -1;
        width += fabs(layout->leading[first] - layout->trailing[next]); first = next;
    }
    return width;
}
int XmTextLayoutCaret(const XmTextLayout *layout, size_t scalar, int affinity, double *x) {
    if (!layout || !x || (affinity != 0 && affinity != 1) ||
        !XmTextUnicodeIsGraphemeBoundary(layout->map, scalar)) return 0;
    *x = affinity ? layout->trailing[scalar] : layout->leading[scalar];
    return isfinite(*x);
}
static size_t caret_lower_bound(const XmTextLayout *layout, double x) {
    size_t lower = 0, upper = layout->caret_count;
    while (lower < upper) {
        size_t middle = lower + (upper - lower) / 2;
        if (layout->carets[middle].x < x) lower = middle + 1; else upper = middle;
    }
    return lower;
}
int XmTextLayoutHit(const XmTextLayout *layout, double x, size_t *scalar, int *affinity) {
    size_t best;
    if (!layout || !scalar || !affinity || !isfinite(x) || !layout->caret_count) return 0;
    best = caret_lower_bound(layout, x);
    if (best == layout->caret_count) best = layout->caret_count - 1;
    else if (best && x - layout->carets[best - 1].x <= layout->carets[best].x - x) --best;
    /* Distinct logical bidi boundaries may share a visual point. Keep the
     * same deterministic first-caret tie policy as the captured sort order. */
    best = caret_lower_bound(layout, layout->carets[best].x);
    *scalar = layout->carets[best].scalar; *affinity = layout->carets[best].affinity; return 1;
}
int XmTextLayoutVisualMove(const XmTextLayout *layout, size_t scalar, int affinity, int direction, size_t *next_scalar, int *next_affinity) {
    double x;
    size_t lower, upper;
    if (!layout || !next_scalar || !next_affinity || (direction != -1 && direction != 1) ||
        !XmTextLayoutCaret(layout, scalar, affinity, &x)) return 0;
    if (affinity && fabs(layout->leading[scalar] - layout->trailing[scalar]) <= 0.01) affinity = 0;
    lower = 0; upper = layout->caret_count;
    while (lower < upper) {
        const struct LayoutCaret *caret;
        size_t middle = lower + (upper - lower) / 2;
        caret = &layout->carets[middle];
        if (caret->x < x || (caret->x == x &&
            (caret->scalar < scalar || (caret->scalar == scalar && caret->affinity < affinity)))) lower = middle + 1;
        else upper = middle;
    }
    if (lower >= layout->caret_count || layout->carets[lower].scalar != scalar ||
        layout->carets[lower].affinity != affinity) return 0;
    if (direction < 0 && lower) --lower;
    else if (direction > 0 && lower + 1 < layout->caret_count) ++lower;
    *next_scalar = layout->carets[lower].scalar; *next_affinity = layout->carets[lower].affinity; return 1;
}
Region XmTextLayoutSelectionRegion(const XmTextLayout *layout, double x, size_t first, size_t last,
                                   const XRectangle *viewport) {
    Region selection, clip;
    size_t scalar;
    if (!layout || !viewport || first > last || last > XmTextUnicodeScalarCount(layout->map) ||
        !XmTextUnicodeIsGraphemeBoundary(layout->map, first) || !XmTextUnicodeIsGraphemeBoundary(layout->map, last) || !isfinite(x)) return NULL;
    selection = XCreateRegion(); clip = XCreateRegion();
    if (!selection || !clip) { if (selection) XDestroyRegion(selection); if (clip) XDestroyRegion(clip); return NULL; }
    XUnionRectWithRegion((XRectangle *)viewport, clip, clip);
    for (scalar = first; scalar < last;) {
        size_t next;
        XRectangle rectangle;
        double left, right;
        if (!XmTextUnicodeNextGrapheme(layout->map, scalar, &next) || next <= scalar) break;
        left = fmin(layout->leading[scalar], layout->trailing[next]) + x;
        right = fmax(layout->leading[scalar], layout->trailing[next]) + x;
        if (right >= viewport->x && left <= (double)viewport->x + viewport->width) {
            left = fmax(left, viewport->x); right = fmin(right, (double)viewport->x + viewport->width);
            rectangle.x = (short)floor(left); rectangle.y = viewport->y;
            rectangle.width = (unsigned short)ceil(right - floor(left)); rectangle.height = viewport->height;
            XUnionRectWithRegion(&rectangle, selection, selection);
        }
        scalar = next;
    }
    XIntersectRegion(selection, clip, selection);
    XDestroyRegion(clip);
    return selection;
}
int XmTextLayoutDraw(const XmTextLayout *layout, XftDraw *draw, const XftColor *color, double x, double baseline,
                     size_t first, size_t last, const XRectangle *viewport) {
    Region selection;
    size_t i;
    XftGlyphFontSpec *specs;
    int count = 0;
    if (!layout || !draw || !color || !viewport || first > last || last > XmTextUnicodeScalarCount(layout->map) ||
        !XmTextUnicodeIsGraphemeBoundary(layout->map, first) || !XmTextUnicodeIsGraphemeBoundary(layout->map, last) || !isfinite(x) || !isfinite(baseline)) return 0;
    if (first == 0 && last == XmTextUnicodeScalarCount(layout->map)) {
        selection = XCreateRegion();
        if (selection) XUnionRectWithRegion((XRectangle *)viewport, selection, selection);
    } else selection = XmTextLayoutSelectionRegion(layout, x, first, last, viewport);
    if (!selection) return 0;
    if (layout->glyph_count > INT_MAX || layout->glyph_count > SIZE_MAX / sizeof(XftGlyphFontSpec)) { XDestroyRegion(selection); return 0; }
    specs = (XftGlyphFontSpec *)malloc((layout->glyph_count + 1) * sizeof(XftGlyphFontSpec));
    if (!specs) { XDestroyRegion(selection); return 0; }
    for (i = 0; i < layout->glyph_count; ++i) {
        double gx = x + layout->glyphs[i].x, gy = baseline + layout->glyphs[i].y;
        if (!layout->glyphs[i].visible) continue;
        if (gx < SHRT_MIN || gx > SHRT_MAX || gy < SHRT_MIN || gy > SHRT_MAX) continue;
        specs[count++] = (XftGlyphFontSpec){layout->glyphs[i].font, layout->glyphs[i].glyph, (short)lround(gx), (short)lround(gy)};
    }
    XftDrawSetClip(draw, selection); XftDrawGlyphFontSpec(draw, color, specs, count);
    XftDrawSetClip(draw, NULL); XDestroyRegion(selection); free(specs); return 1;
}
