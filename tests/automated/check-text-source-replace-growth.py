#!/usr/bin/env python3
"""Ensure text-source edit growth rejects overflow and realloc failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("static XmTextStatus \nReplace")
end = text.index("\n#define Increment", start)
block = text[start:end]

assert "delta > (long)INT_MAX - data->length" in block
assert "delta < -(long)data->length" in block
assert "resized_ptr = XtRealloc" in block
assert "if (resized_ptr == NULL)" in block
assert "UINT_MAX / (size_t)char_size" in block
assert "_XmTextEnableRedisplay(data->widgets[i]);" in block

print("text-source replace growth guards: ok")
