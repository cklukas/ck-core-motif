#!/usr/bin/env python3
"""Check XmToggleSwitch Redisplay handles failed GCs and small geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleSwitch.c").read_text()
start = source.rindex("static void Redisplay(")
end = source.index("static Boolean SetValues(", start)
function = source[start:end]

assert "knob_gc = sw->toggle_switch.knob_gc ?" in function
assert "!sw->toggle_switch.track_gc || !knob_gc || !label_gc" in function
assert "sw->core.width > 2 * sw->primitive.highlight_thickness" in function
assert "sw->core.height > 2 * sw->primitive.highlight_thickness" in function
print("PASS ToggleSwitch redisplay guards")
