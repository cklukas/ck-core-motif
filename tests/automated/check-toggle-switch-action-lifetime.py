#!/usr/bin/env python3
"""Check ToggleSwitch action does not touch a widget after callbacks run."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleSwitch.c").read_text(encoding="utf-8")
start = source.index("static void Toggle(Widget widget")
end = source.index("Boolean XmToggleSwitchGetState", start)
region = source[start:end]
assert "XmToggleSwitchSetState(widget," in region
assert "XtIsRealized(widget)" not in region
assert "XClearArea" not in region
print("PASS ToggleSwitch action avoids post-callback widget access")
