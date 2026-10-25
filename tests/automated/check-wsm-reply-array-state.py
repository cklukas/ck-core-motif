#!/usr/bin/env python3
"""Ensure WSM reply packers validate all variable-array states."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
for field in (
    "reply->get_state.win_info_list == NULL",
    "reply->register_window.window_data == NULL",
    "reply->wm_windows.win_entry_list == NULL",
):
    assert field in text, field
print("WSM reply packers validate variable-array state")
