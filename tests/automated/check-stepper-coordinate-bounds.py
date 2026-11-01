#!/usr/bin/env python3
from pathlib import Path
source=(Path(__file__).parents[2]/"lib/Xm/Stepper.c").read_text()
s="".join(source.split())
assert "width=MIN(s->core.width,SHRT_MAX),height=MIN(s->core.height,SHRT_MAX)" in s
print("PASS stepper-coordinate-bounds")
