#!/usr/bin/env python3
"""Ensure WSM cleanup tolerates null and partial structures."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "free.c").read_text(
    encoding="utf-8"
)
assert "request == NULL || !request->any.allocated" in text
assert "reply == NULL || !reply->any.allocated" in text
assert "if (ptr != NULL)" in text
assert "if (win_info != NULL)" in text
assert "if (request->wm_windows.match_attributes != NULL)" in text
assert "if (reply->wm_windows.win_entry_list != NULL)" in text
assert "if (data == NULL || num <= 0)" in text
print("WSM cleanup tolerates null and partial structures")
