#!/usr/bin/env python3
"""Check segment-encoding APIs validate their font-list tag."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ResEncod.c").read_text()
register_start = source.index("XmRegisterSegmentEncoding(")
register_end = source.index("XtPointer \n_XmGetEncodingRegistryTarget", register_start)
register = source[register_start:register_end]
lookup_start = source.index("XmMapSegmentEncoding(char")
lookup_end = source.index("\f", lookup_start)
lookup = source[lookup_start:lookup_end]

assert "if (fontlist_tag == NULL)" in register
assert "if (fontlist_tag == NULL)" in lookup
assert "return NULL;" in register
assert "return NULL;" in lookup

print("PASS segment-encoding APIs reject null tags")
