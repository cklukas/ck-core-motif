#!/usr/bin/env python3
"""Check MWM rejects non-positive double-click resource values."""

from pathlib import Path


source = Path("clients/mwm/WmEvent.c").read_text(encoding="utf-8")

assert source.count("wmGD.doubleClickTime > 0") == 2
assert source.count("timeDiff < (Time)wmGD.doubleClickTime") == 2
assert "timeDiff < wmGD.doubleClickTime" not in source

print("PASS MWM double-click comparisons reject non-positive signed thresholds")
