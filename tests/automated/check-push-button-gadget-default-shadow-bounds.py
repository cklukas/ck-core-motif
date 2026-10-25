#!/usr/bin/env python3
"""Check XmPushButtonGadget validates the selected shadow delta."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushBG.c").read_text()
start = source.index("static void \nDrawDefaultButtonShadows(")
end = source.index("static XmImportOperator", start)
shadow = source[start:end]

assert "if (default_button_shadow_thickness > 0)" in shadow
assert "pb->rectangle.width <= 2 * delta" in shadow
assert "pb->rectangle.height <= 2 * delta" in shadow
assert shadow.index("pb->rectangle.width <= 2 * delta") < shadow.index("width = pb->rectangle.width - 2 * delta")
print("PASS PushButtonGadget default shadow bounds")
