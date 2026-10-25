#!/usr/bin/env python3
"""Ensure WSM window-data lookup rejects null lists safely."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "util.c").read_text(
    encoding="utf-8"
)
start = text.index("_WSMGetMatchingWinData")
end = text.index("\n}\n", start)
function = text[start:end]
assert "if (win_data_top == NULL || num <= 0)" in function
assert "return(NULL);" in function
print("WSM window-data lookup handles null lists")
