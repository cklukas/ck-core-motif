#!/usr/bin/env python3
"""Check Container traversal activity validation handles malformed nodes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("NodeIsActive(\n")
region = source[start:source.index("\n}", start)]
assert region.index("if (!node || node->widget_ptr == NULL)") < region.index("XtIsManaged")
assert region.index("if (cw == NULL)") < region.index("CtrLayoutIsOUTLINE_DETAIL")
assert region.index("if (c == NULL)") < region.index("c->visible_in_outline")
print("PASS Container node activity validation handles malformed nodes")
