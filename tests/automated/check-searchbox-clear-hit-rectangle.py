#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/SearchBox.c").read_text()
s = "".join(source.split())
assert "x>=rect.x&&x<rect.x+rect.width&&y>=(int)border&&y<(int)box->core.height-(int)border" in s
assert s.count("clear_hit(box,event->xbutton.x,event->xbutton.y)")==2
assert "intx,y;" in s
assert "if(x<SHRT_MIN||x>SHRT_MAX||y<SHRT_MIN||y>SHRT_MAX)" in s
assert "rect->x=(short)x;" in s and "rect->y=(short)y;" in s
print("PASS searchbox-clear-hit-rectangle")
