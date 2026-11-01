#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/ColorPicker.c").read_text()
s = "".join(source.split())
assert "!p||!XmIsColorPicker((Widget)p)||p->core.being_destroyed||!event||event->type!=KeyPress" in s
assert "!p||!XmIsColorPicker((Widget)p)||p->core.being_destroyed||!event||!dispatch" in s
print("PASS colorpicker-event-guards")
