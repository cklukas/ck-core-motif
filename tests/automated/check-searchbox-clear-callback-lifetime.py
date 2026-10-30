#!/usr/bin/env python3
"""Check SearchBox clear snapshots callback state before dispatch."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SearchBox.c").read_text(
    encoding="utf-8"
)
start = source.index("void XmSearchBoxClear(")
end = source.index("void XmSearchBoxCommit(", start)
region = source[start:end]

for needle in (
    "cleared_callbacks = box->search.cleared_callback;",
    "query_callbacks = box->search.query_changed_callback;",
    "query = query_copy(widget);",
    "XtCallCallbackList(widget, cleared_callbacks, &callback);",
    "XtCallCallbackList(widget, query_callbacks, &callback);",
):
    assert needle in region
assert region.index("query_callbacks = box->search.query_changed_callback;") < region.index(
    "XtCallCallbackList(widget, cleared_callbacks, &callback);"
)
assert "box->search.query_changed_callback" not in region.split(
    "XtCallCallbackList(widget, cleared_callbacks, &callback);", 1
)[1]

print("PASS SearchBox clear snapshots callbacks before dispatch")
