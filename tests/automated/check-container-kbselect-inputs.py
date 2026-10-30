#!/usr/bin/env python3
"""Check Container keyboard selection validates widget, event, and focus state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("KBSelect(\n")
region = source[start:source.index("\n}", start)]
assert region.index("if (wid == NULL || event == NULL)") < region.index("XmGetFocusWidget")
assert region.index("if (c == NULL)") < region.index("c->selection_state")
assert region.index("if (wid == NULL || event == NULL)") < region.index("event->xbutton.time")
print("PASS Container keyboard selection validates widget, event, and focus state")
