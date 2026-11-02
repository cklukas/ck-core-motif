#!/usr/bin/env python3
"""Ensure WSM packers reject non-empty allocation failure."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
for marker in ("_WSMPackRequest", "_WSMPackReply"):
    start = text.index(marker)
    end = text.index("\n}\n", start)
    function = text[start:end]
    assert "if (size < 0 || (size_t)size > UINT_MAX)" in function
    assert "save = data = (MessageData) XtMalloc((Cardinal)size);" in function
    assert "if (save == NULL && size != 0)" in function
    assert "return(NULL);" in function
print("WSM packers handle non-empty allocation failure")
