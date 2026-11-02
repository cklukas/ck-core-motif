#!/usr/bin/env python3
"""Verify the Font Selector demo's C23-visible callback contracts."""

from pathlib import Path


root = Path(__file__).resolve().parents[2]
callbacks = (root / "demos" / "programs" / "FontSel" / "callbacks-c.c").read_text(
    encoding="utf-8"
)
creation = (root / "demos" / "programs" / "FontSel" / "creation-c.c").read_text(
    encoding="utf-8"
)
main = (root / "demos" / "programs" / "FontSel" / "fontsel.c").read_text(
    encoding="utf-8"
)

callbacks_expected = (
    "CreateHypeLabel(Widget w, XtPointer client, XtPointer call)",
    "ShowFontValChCB(Widget w, XtPointer client, XtPointer call)",
    "ExplainCB(Widget w, XtPointer client, XtPointer call)",
    "ShowCurFont(Widget w, XtPointer client, XtPointer call)",
    "ShowOtherCB(Widget w, XtPointer client, XtPointer call)",
    "QuitCB(Widget w, XtPointer client, XtPointer call)",
)

for contract in callbacks_expected:
    assert contract in callbacks
assert "(int)(intptr_t)client" in callbacks
assert "CreateHypeLabel(w, client, call)" not in callbacks
assert "CreateDemoForm(Widget parent)" in creation
assert "extern void ShowFontValChCB(Widget, XtPointer, XtPointer);" in creation
assert "main(int argc, char **argv)" in main
assert "main(argc, argv)" not in main

print("Font Selector demo uses typed C23 callback and entry-point contracts")
