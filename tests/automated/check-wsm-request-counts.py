#!/usr/bin/env python3
"""Ensure WSM request packers reject unrepresentable list counts."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
assert "#define WSM_MAX_LIST_ITEMS ((int)(CARD16)~(CARD16)0)" in text
for field in (
    "request->connect.num_versions < 0",
    "request->extensions.num_extensions < 0",
    "request->set_state.num_win_info_list < 0",
    "request->set_state.win_info_list[i].num_data_list < 0",
    "request->wm_windows.num_window_properties < 0",
    "request->wm_windows.num_match_attributes < 0",
    "request->wm_windows.match_attributes[i]->num_attributes < 0",
):
    assert field in text, field
for field in (
    "request->connect.num_versions > WSM_MAX_LIST_ITEMS",
    "request->extensions.num_extensions > WSM_MAX_LIST_ITEMS",
    "request->set_state.num_win_info_list > WSM_MAX_LIST_ITEMS",
    "request->set_state.win_info_list[i].num_data_list > WSM_MAX_LIST_ITEMS",
    "request->wm_windows.num_window_properties > WSM_MAX_LIST_ITEMS",
    "request->wm_windows.num_match_attributes > WSM_MAX_LIST_ITEMS",
    "request->wm_windows.match_attributes[i]->num_attributes > WSM_MAX_LIST_ITEMS",
):
    assert field in text, field
print("WSM request packers reject unrepresentable counts")
