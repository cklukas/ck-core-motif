#!/usr/bin/env python3
"""Ensure Spinner event and timer callbacks validate teardown inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Spinner.c").read_text(
    encoding="utf-8"
)
events_start = source.index("static void Events(Widget widget")
tick_start = source.index("static void Tick(XtPointer data")
events = source[events_start:source.index("\n}\n", events_start)]
tick = source[tick_start:source.index("\n}\n", tick_start)]
assert "if (widget == NULL || event == NULL || !XmIsSpinner(widget))" in events
assert "if (spinner == NULL)" in tick
print("Spinner event and timer callbacks guard teardown inputs")
