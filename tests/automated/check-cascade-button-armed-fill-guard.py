#!/usr/bin/env python3
"""Check XmCascadeButton validates its etched-in armed fill."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeB.c").read_text()
start = source.index("static void \nRedisplay(")
redisplay = source[start:start + 2600]

assert "CB_ArmGC(cb) != NULL" in redisplay
assert "cb->core.width != 0 && cb->core.height != 0" in redisplay
print("PASS CascadeButton armed fill guard")
