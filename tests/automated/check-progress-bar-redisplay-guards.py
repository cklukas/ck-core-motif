#!/usr/bin/env python3
"""Check XmProgressBar Redisplay handles partial GC allocation safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressBar.c").read_text()
update = source[source.index("static void UpdateGCs"):source.rindex("static void Initialize")]
start = source.rindex("static void Redisplay(")
end = source.index("static Boolean SetValues(", start)
function = source[start:end]

assert "if (progress->progress.track_gc)" in update
assert "if (!progress->progress.track_gc || !progress->progress.fill_gc)" in function
assert "if (!text_gc) text_gc = progress->progress.track_gc;" in function
assert "progress->core.width > 2 * progress->primitive.highlight_thickness" in function
print("PASS ProgressBar redisplay guards")
