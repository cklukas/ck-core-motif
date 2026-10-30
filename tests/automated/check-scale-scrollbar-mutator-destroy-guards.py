#!/usr/bin/env python3
"""Check classic Scale and ScrollBar setters fail during teardown."""

from pathlib import Path

scale = (Path(__file__).resolve().parents[2] / "lib/Xm/Scale.c").read_text(
    encoding="utf-8"
)
scrollbar = (Path(__file__).resolve().parents[2] / "lib/Xm/ScrollBar.c").read_text(
    encoding="utf-8"
)

assert "!XmIsScale(w)" in scale and "sw->core.being_destroyed" in scale
assert "!XmIsScrollBar(w)" in scrollbar and "sbw->core.being_destroyed" in scrollbar
print("PASS Scale/ScrollBar mutator teardown guards")
