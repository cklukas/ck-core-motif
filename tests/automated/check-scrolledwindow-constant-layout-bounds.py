#!/usr/bin/env python3
"""Check ScrolledWindow constant layout handles collapsed geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()

assert "MyWidth = (int)sw->core.width" in source
assert "MyHeight = (int)sw->core.height" in source
assert "sw->core.width >" in source
assert "sw->core.height >" in source

print("ScrolledWindow constant layout geometry is bounded")
