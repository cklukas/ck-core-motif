#!/usr/bin/env python3
"""Ensure TeachingTip event callbacks validate closure and event inputs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TeachingTip.c").read_text(
    encoding="utf-8"
)
for name, marker in (("owner_events", "static void owner_events(Widget owner"),
                     ("popup_events", "static void popup_events(Widget popup")):
    start = source.index(marker)
    end = source.index("\nstatic void ", start + len(marker))
    body = source[start:end]
    assert "event == NULL" in body, name
    assert "XmIsTeachingTip((Widget)tip)" in body, name
    assert body.index("event == NULL") < body.index("event->type"), name
print("PASS TeachingTip event callbacks validate closure and event inputs")
