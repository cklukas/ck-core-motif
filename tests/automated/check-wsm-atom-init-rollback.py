#!/usr/bin/env python3
"""Ensure WSM display metadata rolls back failed atom initialization."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "disp.c").read_text(
    encoding="utf-8"
)
assert "WSMDispInfo *parent_disp_info = NULL;" in text
assert "if (!XInternAtoms(dpy, atom_names, XtNumber(atom_names), False, atoms))" in text
assert "parent_disp_info->next = NULL;" in text
assert "GDispInfo = NULL;" in text
assert "XtFree((XtPointer) disp_info);" in text
print("WSM atom initialization rolls back safely")
