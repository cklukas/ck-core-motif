#!/usr/bin/env python3
"""Ensure text selection imports check converted string lengths before narrowing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text(
    encoding="utf-8"
)

assert "static Boolean TextSelectionBlockLength(const char *text" in source
assert "if (text_length > (size_t)INT_MAX)" in source
assert source.count("TextSelectionBlockLength(block.ptr, &block.length)") == 3
assert "block.length = strlen(block.ptr);" not in source

print("Text selection imports bound converted string lengths before narrowing")
