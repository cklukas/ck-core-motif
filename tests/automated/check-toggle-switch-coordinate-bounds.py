#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/ToggleSwitch.c").read_text()
s = "".join(source.split())
assert "right=(int)sw->core.width-m.inset;bottom=(int)sw->core.height-m.inset" in s
assert "staticshortbounded_position(intvalue)" in s
assert "staticunsignedshortbounded_extent(intvalue)" in s
assert "clip=clip_rectangle(left,top,right,bottom)" in s
assert "XSetClipRectangles(d,gc,0,0,&clip,1,Unsorted)" in s
assert "_XmAppearanceFillRoundedRectangle" in s
assert "if(m->knob>m->track_height)m->knob=m->track_height" in s
assert "sw->core.width>2*sw->primitive.highlight_thickness" in s
print("PASS toggle-switch-coordinate-bounds")
