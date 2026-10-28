#!/usr/bin/env python3
"""Ensure AutoSuggestBox callbacks tolerate teardown and null events."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c").read_text(
    encoding="utf-8"
)
query_start = source.index("static void query_changed(Widget widget")
commit_start = source.index("static void search_committed(Widget widget")
selected_start = source.index("static void list_selected(Widget widget")
events_start = source.index("static void search_events(Widget widget")
query = source[query_start:source.index("\n}\n", query_start)]
commit = source[commit_start:source.index("\n}\n", commit_start)]
selected = source[selected_start:source.index("\n}\n", selected_start)]
events = source[events_start:source.index("\n}\n", events_start)]
list_events_start = source.index("static void list_events(Widget widget")
list_events = source[list_events_start:source.index("\n}\n", list_events_start)]
assert "if (box == NULL || !XmIsAutoSuggestBox((Widget)box))" in query
assert "if (box == NULL || !XmIsAutoSuggestBox((Widget)box))" in commit
assert "box != NULL && XmIsAutoSuggestBox((Widget)box) && selection" in selected
assert "if (box == NULL || event == NULL || dispatch == NULL" in events
assert "!XmIsAutoSuggestBox((Widget)box)" in events
assert "if (box == NULL || event == NULL || !XmIsAutoSuggestBox" in list_events
print("AutoSuggestBox callbacks guard teardown and null events")
