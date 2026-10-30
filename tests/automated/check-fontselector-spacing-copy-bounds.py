#!/usr/bin/env python3
"""Check fixed-width FontSelector spacing fields use bounded copies."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
for field in ("fixed_spacing", "cf->spacing"):
    assert f"strcpy({field}" not in source
assert "StoreString(font->spacing, fam->fixed_spacing, SPACING_LEN);" in source
assert "StoreString(fam->fixed_spacing, cf->spacing, SPACING_LEN);" in source
assert "StoreString(STAR_STRING, cf->spacing, SPACING_LEN);" in source
assert "StoreString(PROPORTIONAL_SPACING, cf->spacing, SPACING_LEN);" in source

print("FontSelector spacing fields use bounded copies")
