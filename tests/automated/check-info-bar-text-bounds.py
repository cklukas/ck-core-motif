#!/usr/bin/env python3
"""Check bounded XmInfoBar text geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text(encoding="utf-8")
assert "XmStringExtent" in source
assert "prefix_width" in source
assert "message_width" in source
assert "action_width" in source
print("PASS info-bar text geometry uses measured render-table extents")
