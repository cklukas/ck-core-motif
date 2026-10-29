#!/usr/bin/env python3
"""Check Toolbar and StatusBar layout totals saturate before comparison."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
for name in ("Toolbar.c", "StatusBar.c"):
    source = (root / name).read_text()
    assert "layout_total_add(unsigned long total" in source, name
    assert "ULONG_MAX - total" in source, name
    assert "layout_total_add(total," in source, name
    assert "scratch_overflow" in source, name
    assert "UINT_MAX / sizeof(*chosen)" in source, name
    assert "if (child == NULL)" in source, name
    assert "*width = 0;" in source, name
    assert "*height = 0;" in source, name
    if name == "Toolbar.c":
        assert "UINT_MAX / sizeof(*action_width)" in source
        assert "UINT_MAX / sizeof(*action_height)" in source
        assert "if (action->menu_button != NULL)" in source
print("PASS composite layout totals are overflow-safe")
