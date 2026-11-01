#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/ToggleSwitch.c").read_text()
s = "".join(source.split())
body=s[s.index("staticvoidToggle(Widgetw,"):s.index("staticvoidArm(Widgetw,")]
assert "set_state(w," in body
assert "redraw(" not in body
body=s[s.index("staticvoidset_state("):s.index("staticvoidToggle(Widgetw,")]
assert body.index("redraw(widget)") < body.index("_XmCallCallbacksUntilDestroyed")
print("PASS toggle-switch-action-lifetime")
