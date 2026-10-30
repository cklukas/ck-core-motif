#!/usr/bin/env python3
"""Check the Container scroll timer validates closure and parent state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("ScrollProc(")
region = source[start:source.index("\n}", start)]
assert region.index("if (closure == NULL)") < region.index("cw->container.scroll_proc_id")
assert region.index("if (sf == NULL)") < region.index("XmeTraitGet")
print("PASS Container scroll timer validates closure and scroll-frame parent")
