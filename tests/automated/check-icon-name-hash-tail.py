#!/usr/bin/env python3
"""Check icon-name hashing uses the final character, not the NUL terminator."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("static XmHashValue \nHashIconName")
end = source.index("String\nXmGetIconFileName", start)
function = source[start:end]

assert "size_t tail_index;" in function
assert "tail_index = len == 0 ? 0 : len - 1;" in function
assert "data->key_name[tail_index]" in function
assert "data->key_name[len]" not in function

print("PASS icon-name hash uses a safe final-character index")
