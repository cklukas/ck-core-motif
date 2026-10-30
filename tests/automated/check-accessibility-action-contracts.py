#!/usr/bin/env python3
"""Check value-only widgets do not advertise undispatchable actions."""

from pathlib import Path


source = Path("lib/Xm/Accessible.c").read_text()
color_start = source.rindex("if (XmIsColorPicker(w))")
rating_start = source.rindex("if (XmIsRatingControl(w))")
color = source[color_start : rating_start]
rating = source[rating_start : source.rindex("if (XmIsInfoBar(w))")]

assert "i->action_count = 0;" in color
assert "i->action_count = 0;" in rating
assert "phantom action" in color
assert "no action dispatcher" in rating

print("value-only accessibility widgets advertise no phantom actions")
