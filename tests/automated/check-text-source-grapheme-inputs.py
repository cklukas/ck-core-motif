#!/usr/bin/env python3
"""Check grapheme stepping validates source callbacks and storage."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmStringSourceGraphemeStep(")
end = source.index("\n}", start)
region = source[start:end]

assert "source == NULL || source->Scan == NULL" in region
assert "source->data == NULL || source->data->widgets == NULL" in region
assert "source->data->numwidgets <= 0" in region
assert "return position;" in region

print("Text grapheme stepping rejects missing source state")
