#!/usr/bin/env python3
"""Check both XmCascadeButton etched-in fill variants validate state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeB.c").read_text()
start = source.index("static void \nRedisplay(")
redisplay = source[start:start + 2600]

assert "CB_ArmGC(cb) != NULL" in redisplay
assert "GC fillGC = NULL" in redisplay
assert "fillGC = CB_IsArmed(cb) ? CB_ArmGC(cb) : CB_BackgroundGC(cb);" in redisplay
assert "if (fillGC != NULL && cb->core.width != 0 && cb->core.height != 0)" in redisplay
print("PASS CascadeButton legacy fill guard")
