#!/usr/bin/env python3
"""Check TextOut allocation failure handling."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()

paragraph = source[source.index("entry = (struct _XmTextParagraphCache *)XtCalloc"):
                   source.index("\n  entry->start", source.index("entry = (struct _XmTextParagraphCache *)XtCalloc"))]
assert "if (!entry)" in paragraph
assert "XmTextParagraphFree(paragraph);" in paragraph

create = source[source.index("tw->text.output = output ="):
                 source.index("data->unicode_font_context", source.index("tw->text.output = output ="))]
assert "if (output == NULL)" in create
assert "if (data == NULL)" in create

baselines = source[source.index("*line_count = data->number_lines;"):
                   source.index("*baselines = base_array;", source.index("*line_count = data->number_lines;"))]
assert "if (*line_count <= 0)" in baselines
assert "if (base_array == NULL)" in baselines
assert source.count("if (*extra)") >= 7

print("TextOut allocation guards are present")
