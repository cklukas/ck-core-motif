#!/usr/bin/env python3
"""Check MWM icon-box GC copies use an explicitly unsigned full mask."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmIconBox.c").read_text(
    encoding="utf-8"
)
assert source.count("copyMask = ~(XtGCMask)0;") == 2
assert "copyMask = ~0L;" not in source
print("PASS MWM icon-box GC masks are explicitly unsigned")
