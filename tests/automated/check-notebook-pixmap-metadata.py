#!/usr/bin/env python3
"""Check Notebook binding pixmap metadata failure handling."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Notebook.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nDrawPixmapBinding")
end = text.find("\f", start)
body = text[start:] if end < 0 else text[start:end]
required = [
    "if (!XmeGetPixmapData(XtScreen(nb), pixmap,",
    "&depth,",
    ")\n\treturn;",
    "if (depth == 1)",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("Notebook metadata guard missing: " + ", ".join(missing))
lookup = body.index("if (!XmeGetPixmapData")
branch = body.index("if (depth == 1)")
if "return;" not in body[lookup:branch]:
    raise SystemExit("Notebook can branch on depth after failed metadata lookup")
print("Notebook binding pixmap metadata guard is present")
