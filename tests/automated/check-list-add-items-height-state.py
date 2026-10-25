#!/usr/bin/env python3
"""Check that XmList captures the old height extent before insertion."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "List.c"
text = source.read_text(encoding="utf-8")
needle = "Dimension old_max_height = lw->list.MaxItemHeight;"
assert needle in text, "APIAddItems must initialize old_max_height"

start = text.index("static void\nAPIAddItems(")
end = text.index("\n}\n", start) + 3
body = text[start:end]
assert needle in body, "initialization must be inside APIAddItems"
assert "SetNewSize(lw, False, False, old_max_height);" in body
print("list add-items height state guard present")
