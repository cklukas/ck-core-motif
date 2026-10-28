#!/usr/bin/env python3
"""Check XPM buffer headers use bounded, type-correct formatting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text()
assert '"%u %u %u %u"' in source
assert "written = snprintf(buf, sizeof(buf)" in source
assert "written = snprintf(ptr + used_size" in source
assert '"/*%s*/\\n", info->colors_cmt' in source
assert 'written = snprintf(buf + l, sizeof(buf) - l, " XPMEXT")' in source
assert 'written = snprintf(buf + l, sizeof(buf) - l, "\\\",\\n")' in source
assert "if (written < 0 || (size_t)written >= sizeof(buf))" in source
assert "ptr_size - used_size" in source
assert "sprintf(buf, \"/* XPM */" not in source
print("XPM buffer headers use bounded, type-correct formatting")
