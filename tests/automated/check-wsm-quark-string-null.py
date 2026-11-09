#!/usr/bin/env python3
"""Ensure WSM config serialization handles invalid quarks."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
assert "WSMQuarkString(XrmQuark quark)" in text
assert "return(string != NULL ? string : \"\");" in text
assert "AddConfigFormatSize" in text
assert "WSMStringLength(WSMQuarkString(" in text
assert "size += strlen(WSMQuarkString" not in text
assert "PackString(data, WSMQuarkString(attr_list->nameq))" in text
print("WSM config serialization handles invalid quarks")
