#!/usr/bin/env python3
"""Check Container outline-button creation validates widget and allocation state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("MakeOutlineButton(\n")
region = source[start:source.index("\n}", start)]
assert region.index("if (cwid == NULL)") < region.index("cw = (XmContainerWidget)")
assert region.index("if (c == NULL)") < region.index("c->outline_state")
assert "if (c->related_cwid == NULL)" in region
assert "if (obc == NULL)" in region
print("PASS Container outline-button creation validates widget and allocation state")
