#!/usr/bin/env python3
"""Check that XmIm bounds the application-owned preedit resource."""
from pathlib import Path

root = Path(__file__).resolve().parents[2]
source = (root / "lib/Xm/XmIm.c").read_text()
assert "if (strlen(cp) < sizeof(tmp))" in source
assert "memcpy(tmp, cp, strlen(cp) + 1);" in source
assert "strcpy(tmp,cp)" not in source
print("XmIm preedit-resource bounds contract passed")
