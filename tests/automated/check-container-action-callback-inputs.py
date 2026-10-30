#!/usr/bin/env python3
"""Check Container default-action callback validates child state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("CallActionCB(\n")
region = source[start:source.index("\n}", start)]
assert region.index("if (cwid == NULL)") < region.index("cw = (XmContainerWidget)XtParent")
assert region.index("if (cw == NULL)") < region.index("GetContainerConstraint")
assert region.index("if (c == NULL)") < region.index("c->selection_state")
print("PASS Container default-action callback validates child state")
