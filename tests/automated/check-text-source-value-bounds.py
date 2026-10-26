#!/usr/bin/env python3
"""Ensure Text source value extraction respects Xt and pointer-width bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text(
    encoding="utf-8"
)

range_start = source.index("TextSourceRangeBytes(")
range_end = source.index("\nstatic Boolean\nTextSourceByteOffset", range_start)
range_region = source[range_start:range_end]
assert "*bytes > (size_t)UINT_MAX" in range_region

start = source.index("_XmStringSourceGetValue(")
end = source.index("\nvoid \n_XmStringSourceSetValue", start)
region = source[start:end]
assert "if (source == NULL || source->data == NULL)" in region
assert "data->widgets == NULL || data->numwidgets <= 0" in region
assert "size_t length = 0;" in region
assert "size_t return_val;" in region
assert "int return_val;" not in region

print("Text source value extraction checks Xt bounds and preserves size_t lengths")
