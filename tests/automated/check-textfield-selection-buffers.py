#!/usr/bin/env python3
"""Check TextField selection exports bound temporary buffers."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
helper_start = source.index("static char *\ntext_field_selection_buffer")
helper_end = source.index("\nstatic void", helper_start)
helper = source[helper_start:helper_end]
convert_start = source.index("_XmTextFieldConvert(")
convert_start = source.rfind("Boolean\n", 0, convert_start)
convert_end = source.index("\n/* ARGSUSED */\nvoid\n_XmTextFieldLoseSelection", convert_start)
convert = source[convert_start:convert_end]

assert "length > (unsigned long)UINT_MAX - 1" in helper
assert convert.count("text_field_selection_buffer(*length)") == 8
assert convert.count("if (tmp_value == NULL)") == 6
assert convert.count("if (*value == NULL)") == 2

print("TextField selection export buffers are bounded and checked")
