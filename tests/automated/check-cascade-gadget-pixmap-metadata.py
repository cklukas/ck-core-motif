#!/usr/bin/env python3
"""Check CascadeButtonGadget pixmap metadata failure handling."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "CascadeBG.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nDrawCascade(")
end = text.find("\f", start)
body = text[start:] if end < 0 else text[start:end]
required = [
    "if (!XmeGetPixmapData(XtScreen(cb),",
    "&depth,",
    ")\n\t      return;",
    "if (depth == XtParent(cb)->core.depth)",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("CascadeButtonGadget metadata guard missing: " + ", ".join(missing))
lookup = body.index("if (!XmeGetPixmapData")
branch = body.index("if (depth == XtParent(cb)->core.depth)")
if "return;" not in body[lookup:branch]:
    raise SystemExit("CascadeButtonGadget can use depth after failed metadata lookup")
print("CascadeButtonGadget pixmap metadata guard is present")
