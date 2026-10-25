#!/usr/bin/env python3
"""Check XmRegion cleanup and rectangle-output APIs tolerate NULL inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
destroy_start = source.index("_XmRegionDestroy(")
destroy_start = source.index("void", destroy_start - 80)
destroy_end = source.index("\n}\n", destroy_start) + 2
destroy = source[destroy_start:destroy_end]
assert "if (r == NULL)" in destroy
assert destroy.index("if (r == NULL)") < destroy.index("r->rects")

get_start = source.index("_XmRegionGetRectangles(")
get_start = source.index("void", get_start - 80)
get_end = source.index("\n}\n", get_start) + 2
get = source[get_start:get_end]
assert "if (rects == NULL || nrects == NULL)" in get
assert "if (r == NULL)" in get
assert get.index("if (rects == NULL || nrects == NULL)") < get.index("r->rects")
assert get.index("*rects = NULL") < get.index("XtMalloc")
print("PASS XmRegion API NULL guards")
