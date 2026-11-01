#!/usr/bin/env python3
from pathlib import Path
source=(Path(__file__).parents[2]/"lib/Xm/Stepper.c").read_text()
s="".join(source.split())
assert "_XmAppearanceInheritForeground(children[i],widget)" in s
assert "s->stepper.decrement,s->stepper.label,s->stepper.increment" in s
print("PASS stepper-color-update")
