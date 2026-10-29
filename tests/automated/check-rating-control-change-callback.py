#!/usr/bin/env python3
"""Check RatingControl value callbacks are emitted only for actual changes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RatingControl.c").read_text()
start = source.index("void XmRatingControlSetValue")
end = source.index("Widget XmCreateRatingControl", start)
function = source[start:end]
assert "int old" in function
assert "old != rating->rating.value" in function
print("RatingControl value callbacks require an actual normalized-value change")
