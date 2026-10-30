#!/usr/bin/env python3
"""Check Container marquee selection validates child and constraint inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
mark_start = source.rindex("MarkCwidsInMarquee(\n")
mark = source[mark_start:source.index("\n}", mark_start)]
assert "if (node->widget_ptr == NULL)" in mark
assert "if (c == NULL)" in mark
in_start = source.rindex("InMarquee(\n\tWidget")
inside = source[in_start:source.index("\n}", in_start)]
assert inside.index("if (cwid == NULL)") < inside.index("XtVaGetValues")
assert inside.index("if (cw == NULL)") < inside.index("XtVaGetValues")
print("PASS Container marquee selection validates child and constraint inputs")
