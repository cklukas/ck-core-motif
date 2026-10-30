#!/usr/bin/env python3
"""Check scrolled-list construction does not callback a failed child."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("XmCreateScrolledList(")
end = source.index("/* Selection API", start)
region = source[start:end]
sw = region.index("sw = XtCreateManagedWidget")
child = region.index("lw = XtCreateWidget", sw)
assert "if (sw == NULL)" in region[sw:child]
assert "return NULL;" in region[sw:child]
assert "if (lw == NULL)" in region[child:]
assert region.index("if (lw == NULL)") < region.index("XtAddCallback", child)
print("PASS scrolled-list construction handles failed widgets safely")
