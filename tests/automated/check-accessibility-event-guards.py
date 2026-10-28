#!/usr/bin/env python3
"""Ensure accessibility event hooks validate callback inputs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text(
    encoding="utf-8"
)
for marker in ("static void widget_event", "static void native_key_event"):
    start = source.index(marker)
    end = source.index("\nstatic ", start + len(marker))
    body = source[start:end]
    assert "a == NULL || w == NULL || event == NULL" in body, marker
    assert body.index("a == NULL || w == NULL || event == NULL") < body.index("event->type"), marker
assert "cont != NULL" in source[source.index("static void native_key_event"):]
print("PASS accessibility event hooks validate widget, state, and event inputs")
