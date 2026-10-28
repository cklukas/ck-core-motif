#!/usr/bin/env python3
"""Ensure callback item types are checked before record mutation."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
assert "UrmCWRCallbackItemTypeSupported" in text
start = text.index("UrmCWRSetCallbackItem (")
body = text[start:text.index("\n}\n", start)]
check_pos = body.index("UrmCWRCallbackItemTypeSupported")
append_pos = body.index("UrmCWR__AppendString")
assert check_pos < append_pos
assert "MrmBAD_ARG_TYPE" in body[check_pos:append_pos]
print("Mrm callback item types are preflighted before mutation")
