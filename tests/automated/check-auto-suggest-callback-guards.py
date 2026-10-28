#!/usr/bin/env python3
"""Ensure AutoSuggestBox callback closures validate their widget class."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c").read_text(
    encoding="utf-8"
)
for marker, next_marker in (
    ("static void query_changed(Widget widget", "\nstatic void search_committed"),
    ("static void search_committed(Widget widget", "\nstatic void list_selected"),
    ("static void list_selected(Widget widget", "\nstatic void search_events"),
    ("static void search_events(Widget widget", "\nstatic void list_events"),
    ("static void list_events(Widget widget", "\nWidget XmCreateAutoSuggestBox"),
):
    start = source.index(marker)
    body = source[start : source.index(next_marker, start)]
    assert "XmIsAutoSuggestBox((Widget)box)" in body, marker
print("PASS AutoSuggestBox callbacks validate closure widget class")
