#!/usr/bin/env python3
"""Ensure WSM nested window arrays are allocated and freed explicitly."""

from pathlib import Path


pack = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
free = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "free.c").read_text(
    encoding="utf-8"
)
assert "allowed_attributes = NULL;" in pack
assert "prohibited_attributes = NULL;" in pack
assert "(size_t)num * sizeof(WindowProperty)" in pack
assert "allowed_attributes = NULL;" in pack
assert "prohibited_attributes = NULL;" in pack
assert "case WSM_WM_WINDOWS:" in free
assert free.count("request->wm_windows.match_attributes[i]->allowed_attributes") == 1
assert free.count("reply->wm_windows.win_entry_list[i].windows") == 1
print("WSM nested window arrays have explicit ownership")
