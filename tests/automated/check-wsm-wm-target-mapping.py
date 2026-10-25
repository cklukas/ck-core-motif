#!/usr/bin/env python3
"""Ensure WSM WM-specific targets map in both directions."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "util.c").read_text(
    encoding="utf-8"
)
for name in (
    "WSM_WM_GET_BACKGROUND_WINDOW",
    "WSM_WM_SET_BACKGROUND_WINDOW",
    "WSM_WM_WINDOWS",
    "WSM_WM_FOCUS",
    "WSM_WM_POINTER",
):
    assert text.count(name) >= 3, name
print("WSM WM-specific targets map both directions")
