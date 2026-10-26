#!/usr/bin/env python3
"""Check escaped trailing whitespace is distinguished from ordinary padding."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = source.rindex("GetNextToken(")
body = source[start:source.index("/*ARGSUSED*/", start)]
assert "isspace((unsigned char)*e)" in body
assert "*(e - 1) != '\\\\'" in body
assert "(*e-1) != '\\\\'" not in body
print("PASS resource token trimming honors escaped trailing whitespace")
