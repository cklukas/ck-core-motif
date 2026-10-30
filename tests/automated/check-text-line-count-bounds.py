#!/usr/bin/env python3
"""Ensure Text line counting uses bounded source-character traversal."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("static int \nCountLines(XmTextSource source,")
end = text.index("\nstatic void", start)
block = text[start:end]

assert "if (start < 0)" in block
assert "source == NULL || source->data == NULL" in block
newline_start = text.index("SourceNewlineCharacter(XmSourceData data)")
newline_end = text.index("\n/* In a supported UTF-8 locale", newline_start)
newline_block = text[newline_start:newline_end]
assert "data->widgets == NULL || data->numwidgets <= 0" in newline_block
assert "data->PSWC_NWLN == NULL" in newline_block
assert "unsigned long skipped" in block
assert "if (start >= data->length) return 0;" in block
assert "length > (unsigned long)(data->length - start)" in block
assert "SourceNewlineCharacter(data)" in block
assert "SourceLineCharacter(data, position)" in block
assert "(int)length" not in block
assert "data->gap_start - (data->ptr + start)" not in block

print("text line-count range bounds: ok")
