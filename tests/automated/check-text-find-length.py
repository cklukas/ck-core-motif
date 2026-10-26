#!/usr/bin/env python3
"""Check Text search lengths are representable before character counting."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFind.c").read_text()
assert source.count("if (search_string == NULL)") == 2
assert source.count("search_bytes > (size_t)INT_MAX") == 2
assert source.count("_XmTextCountCharacters(search_string, (int)search_bytes)") == 2

print("Text search lengths are checked before character counting")
