#!/usr/bin/env python3
"""Check List bounds conversion validates positions before subtracting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.index("Boolean\nXmListPosToBounds")
end = source.index("Widget\nXmCreateList", start)
body = source[start:end]

guard = body.index("if (position < 0 || position > lw->list.itemCount)")
conversion = body.index("position--;", guard)
assert guard < conversion
assert "position = lw->list.itemCount - 1;" in body
print("List bounds position conversion is range-checked")
