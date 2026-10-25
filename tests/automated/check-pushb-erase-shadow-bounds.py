#!/usr/bin/env python3
"""Check PushButton default-shadow erasure handles collapsed geometry."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "PushB.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("EraseDefaultButtonShadow(")
end = text.index("/************************************************************************\n *\n *  SetValuesPrehook", start)
body = text[start:end]
required = [
    "(int)pb->core.width <= 2 * delta",
    "(int)pb->core.height <= 2 * delta",
    "width = (int)pb->core.width - (2 * delta);",
    "height = (int)pb->core.height - (2 * delta);",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("PushButton erase-shadow guard missing: " + ", ".join(missing))
print("PushButton erase-shadow geometry guard is present")
