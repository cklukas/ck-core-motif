#!/usr/bin/env python3
"""Ensure text-source readers reject positive-length null payloads."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmStringSourceGetString")
end = text.index("\nstatic void \nAddWidget", start)
get_string = text[start:end]
value_start = text.index("_XmStringSourceGetValue")
value_end = text.index("\nvoid \n_XmStringSourceSetValue", value_start)
get_value = text[value_start:value_end]

assert get_string.count("block.ptr == NULL") >= 2
assert get_value.count("block.ptr == NULL") >= 2

print("text-source block payload guards: ok")
