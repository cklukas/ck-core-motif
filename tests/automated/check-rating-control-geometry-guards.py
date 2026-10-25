#!/usr/bin/env python3
"""Check XmRatingControl handles degenerate widget geometry safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RatingControl.c").read_text()
redisplay = source[source.index("static void Redisplay("):source.index("static void choose(")]
events = source[source.rindex("static void Events("):source.rindex("static Boolean SetValues(")]
assert "!window || !rating->rating.gc" in redisplay
assert "segment > 2 * gap ? segment - 2 * gap : segment" in redisplay
assert "rating->core.height > 12" in redisplay
assert "rating->core.width > 2 * rating->primitive.highlight_thickness" in redisplay
assert "if (rating->core.width == 0) return;" in events
print("PASS RatingControl geometry guards")
