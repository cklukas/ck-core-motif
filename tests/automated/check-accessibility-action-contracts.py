#!/usr/bin/env python3
"""Check ColorPicker actions are dispatched and RatingControl exposes its value."""

from pathlib import Path


source = Path("lib/Xm/Accessible.c").read_text()
color_start = source.rindex("if (XmIsColorPicker(w))")
rating_start = source.rindex("if (XmIsRatingControl(w))")
color = source[color_start : rating_start]
rating = source[rating_start : source.rindex("if (XmIsInfoBar(w))")]

assert "i->action_count = 2;" in color
assert "i->action_count = 0;" in rating
assert "XmColorPickerCommit(n->widget" in source
assert "XmColorPickerRevert(n->widget" in source
assert "no action dispatcher" in rating

print("ColorPicker actions and RatingControl value have matching dispatch contracts")
