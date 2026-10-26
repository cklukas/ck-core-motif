#!/usr/bin/env python3
"""Check VirtKeys4DIN bounds its property count and atom allocation."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmInitWs.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean\nVirtKeys4DIN")
end = source.index("\n}", start) + 2
region = source[start:end]
assert "int prop_count;" in region
assert "size_t prop_bytes;" in region
assert "num_props > (unsigned)INT_MAX" in region
assert "(size_t)num_props > (size_t)UINT_MAX / sizeof(Atom)" in region
assert "prop_bytes = (size_t)num_props * sizeof(Atom);" in region
assert "XInternAtoms(dsp, prop_names, prop_count" in region
assert "XInternAtoms(dsp, prop_names, num_props" not in region
print("PASS MWM virtual-key property count and allocation are bounded")
