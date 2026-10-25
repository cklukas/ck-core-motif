#!/usr/bin/env python3
"""Ensure WSM string unpacking handles allocation failure."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
start = text.index("UnpackString(MessageData *data_ptr)")
end = text.index("\n}\n", start)
function = text[start:end]
assert "top = XtMalloc((size_t) len + 1);" in function
assert "if (top == NULL)" in function
assert "return(NULL);" in function
print("WSM UnpackString handles allocation failure")
