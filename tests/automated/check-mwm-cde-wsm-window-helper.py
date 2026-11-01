#!/usr/bin/env python3
"""Check that MWM uses CDE's published WSM window lookup helper."""

from pathlib import Path

source = (
    Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c"
).read_text(encoding="utf-8")

start = source.index("wmGD.requestContextWin")
end = source.index("/* create topmost shell", start)
region = source[start:end]

assert "_DtGetMwmWindow(DISPLAY," in region
assert "&wmGD.dtSmWindow" in region
assert "_DtGetSmWindow" not in source
print("PASS MWM uses the CDE WSM MWM-window helper")
