#!/usr/bin/env python3
"""Check slider callbacks validate navigator traits before reading them."""
from pathlib import Path

root = Path(__file__).parents[2]
for name in ("List.c", "ScrolledW.c"):
    source = (root / "lib" / "Xm" / name).read_text()
    assert source.count("nav_trait == NULL || nav_trait->getValue == NULL") >= 1
    assert source.count("nav_trait->getValue(w, &nav_data)") >= 1
print("PASS navigator slider callbacks are guarded")
