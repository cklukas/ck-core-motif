#!/usr/bin/env python3
"""Ensure Flyout event callbacks validate closure and event inputs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Flyout.c").read_text(
    encoding="utf-8"
)
for name, marker in (("owner_events", "static void owner_events(Widget owner"),
                     ("popup_events", "static void popup_events(Widget popup")):
    start = source.index(marker)
    end = source.index("\nstatic Boolean ", start)
    body = source[start:end]
    assert "event == NULL" in body, name
    assert "XmIsFlyout((Widget)flyout)" in body, name
    assert body.index("event == NULL") < body.index("event->type"), name
print("PASS Flyout event callbacks validate closure and event inputs")
