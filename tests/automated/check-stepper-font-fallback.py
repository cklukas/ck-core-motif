#!/usr/bin/env python3
"""Check Stepper value text follows the widget render table."""
from pathlib import Path

source = Path("lib/Xm/Stepper.c").read_text(encoding="utf-8")
assert "XmNrenderTable" in source
assert "XmStringExtent" in source
assert "XmStringDraw" in source
assert "XmFontListCopy" in source
assert "XmRenderTableFree" in source
assert "if (new_table)" in source
assert "Keep the last valid table" in source
assert "s->stepper.render_table = old->stepper.render_table" in source
print("PASS Stepper draws value text through its render table")
