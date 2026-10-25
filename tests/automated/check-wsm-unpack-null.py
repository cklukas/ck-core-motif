#!/usr/bin/env python3
"""Ensure WSM primitive unpackers tolerate null input streams."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
string_start = text.rindex("UnpackString(MessageData *data_ptr)")
string_end = text.index("/*\tFunction Name: UnpackCARD32", string_start)
assert "if (data_ptr == NULL || *data_ptr == NULL)" in text[string_start:string_end]
card_start = text.rindex("UnpackCARD8(MessageData *data_ptr)")
assert "if (data_ptr == NULL || *data_ptr == NULL)" in text[card_start:]
assert "return(0);" in text[card_start:]
print("WSM primitive unpackers tolerate null input streams")
