#!/usr/bin/env python3
"""Ensure WSM reply packers reject negative counts."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
for field in (
    "reply->extensions.num_extensions < 0",
    "reply->config_format.num_global_formats < 0",
    "reply->config_format.num_window_formats < 0",
    "reply->config_format.num_icon_formats < 0",
    "reply->get_state.num_win_info_list < 0",
    "reply->get_state.win_info_list[i].num_data_list < 0",
    "reply->register_window.num_window_data < 0",
    "reply->wm_windows.num_win_entry_list < 0",
    "reply->wm_windows.win_entry_list[i].num_windows < 0",
    "reply->wm_windows.win_entry_list[i].num_match_properties < 0",
):
    assert field in text, field
print("WSM reply packers reject negative counts")
