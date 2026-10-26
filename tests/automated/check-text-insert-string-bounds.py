#!/usr/bin/env python3
"""Ensure InsertString validates action parameters before length narrowing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nInsertString(")
end = source.index("\nstatic void \nProcessVerticalParams", start)
region = source[start:end]

assert "Cardinal i;" in region
assert "size_t str_length;" in region
assert "if (str == NULL)" in region
assert "str_length = strlen(str);" in region
assert "str_length > (size_t)INT_MAX" in region
assert "block.length = (int)str_length;" in region

print("Text InsertString: action parameters and block lengths are bounded")
