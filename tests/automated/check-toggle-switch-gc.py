#!/usr/bin/env python3
"""Check ToggleSwitch cleans up a partial graphics-context setup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ToggleSwitch.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Initialize(Widget request")
end = source.index("static void Destroy", start)
context = source[start:end]

required = (
    "if (!sw->toggle_switch.track_gc || !sw->toggle_switch.knob_gc ||",
    "XFreeGC(display, sw->toggle_switch.track_gc);",
    "sw->toggle_switch.track_gc = NULL;",
    "sw->toggle_switch.insensitive_gc = NULL;",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("ToggleSwitch GC cleanup missing: " + ", ".join(missing))

print("ToggleSwitch GC cleanup check passed")
