#!/usr/bin/env python3
"""Ensure Text line counting bounds ranges before signed arithmetic."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("CountLines(XmTextSource source,")
end = text.index("\n  /* verify that the 'start'", start)
block = text[start:end]

assert "if (start < 0)" in block
assert "unsigned long skipped" in block
assert "if (start >= data->length) return 0;" in block
assert "length > (unsigned long)(data->length - start)" in block
assert "start + length" not in block

print("text line-count range bounds: ok")
