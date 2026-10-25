#!/usr/bin/env python3
"""Ensure WSM request packers reject negative counts."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
for field in (
    "request->connect.num_versions < 0",
    "request->extensions.num_extensions < 0",
    "request->set_state.num_win_info_list < 0",
    "request->wm_windows.num_window_properties < 0",
    "request->wm_windows.num_match_attributes < 0",
    "request->wm_windows.match_attributes[i]->num_attributes < 0",
):
    assert field in text, field
print("WSM request packers reject negative counts")
