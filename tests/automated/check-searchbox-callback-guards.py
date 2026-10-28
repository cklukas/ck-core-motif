#!/usr/bin/env python3
"""Ensure SearchBox callbacks and events validate teardown inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "SearchBox.c").read_text(
    encoding="utf-8"
)
for name in ("query_changed", "committed"):
    start = source.index(f"static void {name}(Widget widget")
    body = source[start:source.index("\n}", start)]
    assert "box == NULL || !XmIsSearchBox(widget)" in body
start = source.index("static void Events(Widget widget")
body = source[start:source.index("\n}", start)]
assert "box == NULL || event == NULL || !XmIsSearchBox(widget)" in body
print("SearchBox callbacks and events validate teardown inputs")
