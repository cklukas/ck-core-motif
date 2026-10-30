#!/usr/bin/env python3
"""Check XmListPosToBounds validates its widget class early."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(encoding="utf-8")
start = source.index("XmListPosToBounds(Widget")
end = source.index("\n}\n", start)
region = source[start:end]
assert "if (w == NULL || !XmIsList(w))" in region
assert region.index("if (w == NULL || !XmIsList(w))") < region.index("_XmWidgetToAppContext(w)")
print("PASS XmListPosToBounds validates widget class before list access")
