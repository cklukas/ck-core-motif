#!/usr/bin/env python3
"""Check public XmString extraction APIs validate output pointers."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringGet.c").read_text()

assert "if (text == NULL || str_tag == NULL || direction == NULL ||" in source
assert "unknown_tag == NULL || unknown_length == NULL || unknown_value == NULL)" in source
assert "if (text == NULL || !string)" in source

print("PASS XmString extraction APIs validate output pointers")
