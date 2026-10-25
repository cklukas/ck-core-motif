#!/usr/bin/env python3
"""Ensure WSM packers validate nested per-record arrays."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
assert "request->set_state.win_info_list[i].data_list == NULL" in text
assert "reply->get_state.win_info_list[i].data_list == NULL" in text
assert "reply->wm_windows.win_entry_list[i].windows == NULL" in text
assert "reply->wm_windows.win_entry_list[i].match_properties == NULL" in text
print("WSM packers validate nested per-record arrays")
