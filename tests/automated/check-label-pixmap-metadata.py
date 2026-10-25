#!/usr/bin/env python3
"""Check Label pixmap redisplay metadata guards."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Label.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nRedisplay(")
end = text.find("\f", start)
body = text[start:] if end < 0 else text[start:end]
if body.count("if (XmeGetPixmapData(XtScreen(lw)") < 2:
    raise SystemExit("Label redisplay does not guard both pixmap metadata lookups")
if "XmeGetPixmapData(XtScreen(lw), Pix(lw), NULL, &depth" not in body:
    raise SystemExit("Label sensitive pixmap lookup guard missing")
if "XmeGetPixmapData(XtScreen(lw), pix_use, NULL, &depth" not in body:
    raise SystemExit("Label insensitive pixmap lookup guard missing")
lookup = body.index("if (XmeGetPixmapData(XtScreen(lw), pix_use")
following = body[lookup:]
if "if (depth == lw->core.depth)" not in following or "XSetFillStyle" not in following:
    raise SystemExit("Label insensitive draw/stipple path is incomplete")
print("Label pixmap metadata guards are present")
