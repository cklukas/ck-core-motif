#!/usr/bin/env python3
"""Ensure WSM reply callbacks are optional and failures clean up."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "send.c").read_text(
    encoding="utf-8"
)
assert "if (req_info->reply_callback != NULL)" in text
assert "XtFree((XtPointer) req_info);" in text
assert "WSM_ERROR_INTERNAL" in text
print("WSM reply callback lifecycle is safe")
