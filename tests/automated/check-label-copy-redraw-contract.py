#!/usr/bin/env python3
"""Check failed ProgressBar/Spinner label copies do not claim a redraw."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
for name, marker, guard in (
    ("ProgressBar.c", "static Boolean SetValues", "progress->progress.label"),
    ("Spinner.c", "static Boolean SetValues", "spinner->spinner.label"),
):
    source = (root / name).read_text()
    start = source.rindex(marker)
    end = source.index("static XtGeometryResult QueryGeometry", start)
    body = source[start:end]
    success = body.index("replacement =")
    branch = body.index("if (" + guard + " == NULL || replacement != NULL)", success)
    assert "redraw = True;" in body[branch : body.index("} else", branch)]
    failure = body.index("else", branch)
    assert "redraw = True;" not in body[failure : body.index("}", failure) + 1]

print("ProgressBar and Spinner redraw only after successful label updates")
