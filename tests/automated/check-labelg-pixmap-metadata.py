#!/usr/bin/env python3
"""Check LabelG pixmap redisplay metadata guards."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "LabelG.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmRedisplayLabG(Widget wid,")
end = text.find("\f", start)
body = text[start:] if end < 0 else text[start:end]
required = [
    "if (LabelGGetPixmapData(XtScreen(lw), Pix(lw), &depth",
    "if (LabelGGetPixmapData(XtScreen(lw), pix_use, &depth",
    "if (depth == XtParent(lw)->core.depth)",
    "XSetFillStyle(XtDisplay(lw), gc, FillStippled)",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("LabelG metadata guard missing: " + ", ".join(missing))

start = text.index("\nLabelGGetPixmapData(")
helper = text[start:text.index("\n}", start)]
assert "if (XmeGetPixmapData(" in helper
assert "if (!XGetGeometry(" in helper
assert "return False;" in helper[helper.index("if (!XGetGeometry("):]
print("LabelG pixmap metadata guards are present")
