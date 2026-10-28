#!/usr/bin/env python3
"""Ensure XmString unparsing checks mblen failure before rendering."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/XmString.c").read_text()
start = source.index("/*\n * unparse_components:")
end = source.index("\nXmStringUnparse(", start)
body = source[start:end]
assert "int pattern_length = mblen" in body
assert "if (pattern_length > 0)" in body
assert "mblen((char*) pat->pattern, MB_CUR_MAX)," not in body
print("XmString unparse checks multibyte pattern lengths")
