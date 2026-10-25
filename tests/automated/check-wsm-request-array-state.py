#!/usr/bin/env python3
"""Ensure WSM request packers validate variable-array state."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
for field in (
    "request->connect.known_versions == NULL",
    "request->set_state.win_info_list == NULL",
    "request->wm_windows.window_properties == NULL",
    "request->wm_windows.match_attributes == NULL",
    "request->wm_windows.match_attributes[i] == NULL",
):
    assert field in text, field
print("WSM request packers validate variable-array state")
