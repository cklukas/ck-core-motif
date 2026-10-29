#!/usr/bin/env python3
"""Check Toolbar and StatusBar layout totals saturate before comparison."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
for name in ("Toolbar.c", "StatusBar.c"):
    source = (root / name).read_text()
    assert "layout_total_add(unsigned long total" in source, name
    assert "ULONG_MAX - total" in source, name
    assert "layout_total_add(total," in source, name
print("PASS composite layout totals are overflow-safe")
