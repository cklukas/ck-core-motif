#!/usr/bin/env python3
"""Check Paned child spans remain bounded in collapsed containers."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text()

assert "PanedAvailableSpan(Dimension total, int offset, Dimension border)" in source
assert "(available > 0) ? available : 1" in source
assert "PanedAvailableSpan(pw->core.width, offset" in source
assert "PanedAvailableSpan(pw->core.height, offset" in source

print("Paned child spans are bounded")
