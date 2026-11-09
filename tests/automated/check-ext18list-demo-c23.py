#!/usr/bin/env python3
"""Verify Ext18List demo callback contracts and toggle timing."""

from pathlib import Path


root = Path(__file__).resolve().parents[2]
callbacks = (root / "demos" / "programs" / "Ext18List" / "callbacks-c.c").read_text(
    encoding="utf-8"
)
creation = (root / "demos" / "programs" / "Ext18List" / "creation.c").read_text(
    encoding="utf-8"
)
main = (root / "demos" / "programs" / "Ext18List" / "ext18list.c").read_text(
    encoding="utf-8"
)

assert "QuitCB(Widget w, XtPointer client, XtPointer call)" in callbacks
assert "CreateLabel(Widget w, XtPointer client, XtPointer call)" in callbacks
assert "QuitCB(w, client, call)" not in callbacks
assert "CreateLabel(w, client, call)" not in callbacks
assert "(int)(intptr_t)row_info[i].data" in callbacks
assert "(XtPointer)(intptr_t)playernum" in callbacks
assert "XtAddCallback(unsel_pb, XmNvalueChangedCallback, ToggleFindArea," in creation
assert "XmNarmCallback, ToggleFindArea" not in creation
assert "extern void ChoosePlayerCB(Widget, XtPointer, XtPointer);" in creation
assert "InitializeData(Widget shell)" in main

print("Ext18List demo uses typed callbacks and updates finder visibility after toggling")
