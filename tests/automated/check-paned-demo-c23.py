#!/usr/bin/env python3
"""Verify the Paned demo's typed factories and sash controls."""

from pathlib import Path


root = Path(__file__).resolve().parents[2]
creation = (root / "demos" / "programs" / "Paned" / "creation-c.c").read_text(
    encoding="utf-8"
)
main = (root / "demos" / "programs" / "Paned" / "paned.c").read_text(
    encoding="utf-8"
)

assert "Createpaned(Widget parent)" in creation
assert "Createform(Widget parent)" in creation
assert "Createpaned( parent)" not in creation
assert "Createform( parent)" not in creation
third_sash = creation.index('sash_three_tog = XtCreateWidget("sash_three_tog",')
third_sash_end = creation.index("XtManageChild(sash_three_tog);", third_sash)
third_sash_block = creation[third_sash:third_sash_end]
assert "xmToggleButtonWidgetClass" in third_sash_block
assert "XmNvalueChangedCallback, SashValChCB" in third_sash_block
assert "xmLabelWidgetClass" not in third_sash_block
assert "InitControls(void)" in main
assert "(Boolean)(uintptr_t)client" in main
assert "(unsigned char)(uintptr_t)client" in main

print("Paned demo has typed factories and three functional sash toggle controls")
