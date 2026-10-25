#!/usr/bin/env python3
"""Check CascadeButton handles failed pixmap metadata lookups."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "CascadeB.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nDrawCascade(")
end = text.find("\f", start)
body = text[start:] if end < 0 else text[start:end]
required = [
    "if (!XmeGetPixmapData(XtScreen(cb),",
    "&depth,",
    ")\n\t  return;",
    "if (depth == cb->core.depth)",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("CascadeButton metadata guard missing: " + ", ".join(missing))

lookup = body.index("if (!XmeGetPixmapData")
copy = body.index("if (depth == cb->core.depth)")
if body[lookup:copy].find("return;") < 0:
    raise SystemExit("CascadeButton can use depth after a failed metadata lookup")

print("CascadeButton pixmap metadata failure guard is present")
