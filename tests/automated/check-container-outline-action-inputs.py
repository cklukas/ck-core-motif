#!/usr/bin/env python3
"""Check the Container outline action validates parent and constraint state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("OutlineButtonAction(")
region = source[start:source.index("\n}", start)]
guard = region.index("if (cw == NULL || c == NULL)")
assert guard < region.index("c->outline_state = new_state"), "outline action dereferences state before its guard"
assert guard < region.index("XtCallCallbackList"), "outline action uses parent before its guard"
print("PASS Container outline action validates parent and constraint state")
