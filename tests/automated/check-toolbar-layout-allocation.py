#!/usr/bin/env python3
"""Check Toolbar layout contains an allocation-failure guard."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text()
start = source.index("static void layout(XmToolbarWidget toolbar)")
end = source.index("static void negotiate", start)
layout = source[start:end]

assert "chosen == NULL || action_width == NULL || action_height == NULL" in layout
assert 'XmToolbar: unable to allocate layout scratch storage' in layout
assert "toolbar->toolbar.visible_count = 0;" in layout
assert "toolbar->toolbar.overflow_count = toolbar->toolbar.action_count;" in layout
assert layout.index("chosen == NULL || action_width == NULL || action_height == NULL") < layout.index("visible = choose_actions")
print("PASS Toolbar layout allocation failure is contained")
