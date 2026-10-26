#!/usr/bin/env python3
"""Ensure text-source teardown tolerates null and partial source state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmStringSourceDestroy(")
end = source.index("\nchar *\n_XmStringSourceGetValue", start)
region = source[start:end]

assert "if (source == NULL)" in region
assert "if (source->data == NULL)" in region
assert "XtFree((char *)source);" in region
assert "source->data->unicode_map" in region

print("Text source teardown guards null and partial state")
