#!/usr/bin/env python3
"""Check DropDown selection text assembly is exact and linear."""

from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "DropDown.c").read_text(
    encoding="utf-8"
)
start = source.index("SetTextFromList(Widget w)")
body = source[start:source.index("/************************************************************", start)]

assert "separator = i ? 1U : 0U;" in body
assert "char *write_ptr;" in body
assert "write_ptr = combined;" in body
assert "if (i) *write_ptr++ = ',';" in body
assert "memcpy(write_ptr, parts[i], len);" in body
assert "*write_ptr = '\\0';" in body
assert "strcat(combined" not in body

print("DropDown multiple-selection text assembly uses exact linear copies")
