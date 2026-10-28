#!/usr/bin/env python3
"""Check XPM data headers use bounded, type-correct formatting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
region = source[source.index("/* print the hints line */") : source.index("l = (unsigned int)(s - buf + 1)")]

assert '"%u %u %u %u"' in region
assert region.count("written = snprintf(s, sizeof(buf)") == 2
assert region.count("written < 0 ||") == 2
assert "sizeof(buf) - 8" in region
assert "sprintf(s," not in region
print("XPM data headers use bounded, type-correct formatting")
