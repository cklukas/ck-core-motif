#!/usr/bin/env python3
"""Check LabelG pixmap redisplay metadata guards."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "LabelG.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmRedisplayLabG(Widget wid,")
end = text.find("\f", start)
body = text[start:] if end < 0 else text[start:end]
required = [
    "if (XmeGetPixmapData(XtScreen(lw), Pix(lw), &depth".replace(", &depth", ", NULL, &depth"),
    "if (XmeGetPixmapData(XtScreen(lw), pix_use, NULL, &depth",
    "if (depth == XtParent(lw)->core.depth)",
    "XSetFillStyle(XtDisplay(lw), gc, FillStippled)",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("LabelG metadata guard missing: " + ", ".join(missing))
if body.count("if (XmeGetPixmapData(XtScreen(lw)") < 2:
    raise SystemExit("LabelG does not guard both pixmap paths")
print("LabelG pixmap metadata guards are present")
