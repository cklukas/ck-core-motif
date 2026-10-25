#!/usr/bin/env python3
"""Check Text source range sizing and allocation-failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
helper_start = source.index("TextSourceRangeBytes(")
helper_end = source.index("\n}\n\n/* Convert a stream", helper_start)
helper = source[helper_start:helper_end]
assert "(SIZE_MAX / char_size) - 1" in helper
assert "((size_t)length + 1) * char_size" in helper
function_start = source.index("_XmStringSourceGetString(")
function_end = source.index("\n\nstatic void \nAddWidget", function_start)
function = source[function_start:function_end]
assert function.count("if (buf == NULL)") == 2
assert function.count("return NULL;") >= 2
print("PASS Text source range sizing and allocation guards")
