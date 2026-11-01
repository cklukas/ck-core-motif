#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/NumberBox.c").read_text()
s = "".join(source.split())
assert "!number||!XmIsNumberBox((Widget)number)||number->core.being_destroyed||!event||event->type!=KeyPress" in s
print("PASS number-box-event-guards")
