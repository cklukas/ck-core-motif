#!/usr/bin/env python3
"""Check XmSpinner Redisplay handles partial GCs and small geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Spinner.c").read_text()
update_start = source.index("static void UpdateGCs")
update = source[update_start:source.index("static void Redisplay(", update_start)]
start = source.rindex("static void Redisplay(")
end = source.index("static Boolean SetValues(", start)
function = source[start:end]

assert "if (spinner->spinner.ring_gc)" in update
assert "gc = spinner->spinner.ring_gc ? spinner->spinner.ring_gc" in function
assert "if (!gc) return;" in function
assert "int available = (int)spinner->core.height" in function
assert "spinner->core.width > 2 * spinner->primitive.highlight_thickness" in function
print("PASS Spinner redisplay guards")
