#!/usr/bin/env python3
"""Check exported MWM event parsers validate outputs and button indexes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)

button_start = source.rindex("Boolean ParseBtnEvent (")
button_end = source.index("} /* END OF FUNCTION ParseBtnEvent */", button_start)
button = source[button_start:button_end]
assert "if (linePP == NULL || eventType == NULL || button == NULL ||" in button
assert "state == NULL || fClick == NULL)" in button
assert "if (*button >= XtNumber(buttonModifierMasks))" in button
assert button.index("if (*button >= XtNumber(buttonModifierMasks))") < button.index(
    "buttonModifierMasks[*button]"
)

key_start = source.rindex("Boolean ParseKeyEvent (")
key_end = source.index("} /* END OF FUNCTION ParseKeyEvent */", key_start)
key = source[key_start:key_end]
assert "if (linePP == NULL || eventType == NULL || keyCode == NULL ||" in key
assert "state == NULL)" in key

print("PASS exported MWM event parsers validate inputs and button indexes")
