#!/usr/bin/env python3
"""Check DropDown expose geometry preserves signed child origins."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text()

assert "Position x, y;" in source
assert "(int)arrow->core.x - (int)arrow->core.border_width" in source
assert "(int)text->core.x - (int)text->core.border_width" in source
assert "(int)text->core.y - (int)text->core.border_width" in source

print("DropDown expose geometry preserves signed origins")
