#!/usr/bin/env python3
"""Check TraitHash bounds pointer-derived values before signed conversion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Trait.c").read_text(encoding="utf-8")
start = source.index("TraitHash(XmHashKey key)")
region = source[start:]
assert "uintptr_t mixed" in region
assert "mixed % (uintptr_t)INT_MAX" in region
assert "return (XmHashValue)(object ^" not in region
print("PASS TraitHash bounds pointer-derived values before conversion")
