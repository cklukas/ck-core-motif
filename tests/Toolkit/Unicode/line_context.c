/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/TextLayout.h>
#include <fontconfig/fontconfig.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void require(int condition, const char *detail) { if (!condition) { fprintf(stderr, "%s\n", detail); exit(1); } }
int main(void)
{
    Display *display = XOpenDisplay(NULL);
    XftFont *font;
    XmTextParagraph *paragraph;
    XmTextLayout *line, *standalone, *isolated;
    double first, last;
    const char *source = "\xd8\xa8" " abc\txyz";
    const char *lam_alef = "\xd9\x84\xd8\xa7";
    require(display != NULL && XmTextLayoutAvailable(), "Unicode line-context feature/display unavailable");
    font = XftFontOpenName(display, DefaultScreen(display), "DejaVu Sans:size=18");
    require(font != NULL, "line-context fixture font unavailable");
    paragraph = XmTextParagraphCreate(display, font, source, strlen(source), 64);
    require(paragraph != NULL && XmTextParagraphScalarCount(paragraph) == 9, "full paragraph creation/count");
    line = XmTextParagraphLine(paragraph, 2, 9);
    standalone = XmTextLayoutCreateWithTabWidth(display, font, "abc\txyz", 7, 64);
    require(line && standalone, "contextual line capture failed");
    require(XmTextLayoutCaret(line, 0, 0, &first) && XmTextLayoutCaret(line, 7, 0, &last) && first > last,
            "line lost paragraph bidi context before its source range");
    require(XmTextLayoutCaret(standalone, 0, 0, &first) && XmTextLayoutCaret(standalone, 7, 0, &last) && first < last,
            "fixture does not distinguish line context from detached substring");
    XmTextParagraphFree(paragraph);
    require(XmTextLayoutCaret(line, 0, 0, &first), "line did not retain paragraph/font ownership");
    XmTextLayoutFree(line); XmTextLayoutFree(standalone);

    paragraph = XmTextParagraphCreate(display, font, lam_alef, strlen(lam_alef), 64);
    require(paragraph != NULL, "Arabic paragraph creation");
    line = XmTextParagraphLine(paragraph, 0, 1);
    isolated = XmTextLayoutCreate(display, font, "\xd9\x84", 2);
    require(line && isolated && fabs(XmTextLayoutWidth(line) - XmTextLayoutWidth(isolated)) < 0.01,
            "Arabic line-end shaping used letters outside its line boundary");
    XmTextLayoutFree(line); XmTextLayoutFree(isolated); XmTextParagraphFree(paragraph);

    source = "\xd8\xa8 " "\xe2\x81\xa6" "abc\txyz" "\xe2\x81\xa9";
    paragraph = XmTextParagraphCreate(display, font, source, strlen(source), 64);
    require(paragraph != NULL, "isolate paragraph creation");
    line = XmTextParagraphLine(paragraph, 3, 10);
    require(line && XmTextLayoutScalarCount(line) == 7, "line inside an existing isolate was not captured from full context");
    XmTextLayoutFree(line); XmTextParagraphFree(paragraph);

    paragraph = XmTextParagraphCreate(display, font, "e\xcc\x81", 3, 64);
    require(paragraph && !XmTextParagraphLine(paragraph, 0, 1) && !XmTextParagraphLine(paragraph, 1, 2), "line admitted a broken grapheme");
    XmTextParagraphFree(paragraph);
    {
        const char *wrapped = "abc \xd7\x90\xd7\x91\xd7\x92 \xd8\xa8\xd8\xad\xd8\xb1 xyz e\xcc\x81 \xe4\xb8\xad\xe6\x96\x87";
        size_t at = 0, end = 0, lines = 0;
        paragraph = XmTextParagraphCreate(display, font, wrapped, strlen(wrapped), 64);
        require(paragraph != NULL, "wrapped paragraph creation");
        while (at < XmTextParagraphScalarCount(paragraph)) {
            line = XmTextParagraphWrapLine(paragraph, at, 110, &end);
            require(line && end > at && XmTextLayoutScalarCount(line) == end - at, "wrapped line failed to advance");
            require(XmTextLayoutWidth(line) <= 110, "wrapped line overflowed its measured viewport");
            require(XmTextLayoutCaret(line, 0, 0, &first) && XmTextLayoutCaret(line, end - at, 0, &last), "wrapped line lost an endpoint caret");
            at = end; ++lines; XmTextLayoutFree(line);
        }
        require(lines >= 3, "wrap fixture did not compose multiple lines");
        XmTextParagraphFree(paragraph);
    }
    XftFontClose(display, font); XCloseDisplay(display); FcFini();
    puts("Full paragraph bidi/isolate context, line boundary shaping and retained captures passed");
    return 0;
}
