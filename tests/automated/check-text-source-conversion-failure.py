#!/usr/bin/env python3
"""Ensure failed wide text conversion initializes the source buffer."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/TextStrSo.c").read_text()
start = source.index("converted_bytes = value == NULL ? 0 :")
end = source.index("\n    XtFree(tmp_value);", start)
body = source[start:end]
assert "converted_bytes == (size_t)-1" in body
assert "data->length = 0;" in body
assert "data->ptr[0] = '\\0';" in body
assert body.index("data->length = 0;") < body.index("data->ptr[0] = '\\0';")
print("Wide text-source conversion failures initialize an empty buffer")
