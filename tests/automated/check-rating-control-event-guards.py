#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/RatingControl.c").read_text()
s = "".join(source.split())
assert "if(!widget||!event||!XmIsRatingControl(widget)" in s
print("PASS rating-control-event-guards")
