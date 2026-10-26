#!/usr/bin/env python3
"""Check Label pixmap redisplay metadata guards."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Label.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("Redisplay(")
start = text.rfind("static void", 0, start)
end = text.find("\f", start)
body = text[start:] if end < 0 else text[start:end]
if body.count("if (LabelGetPixmapData(XtScreen(lw)") < 2:
    raise SystemExit("Label redisplay does not guard both pixmap metadata lookups")
if "LabelGetPixmapData(XtScreen(lw), Pix(lw), &depth" not in body:
    raise SystemExit("Label sensitive pixmap lookup guard missing")
if "LabelGetPixmapData(XtScreen(lw), pix_use, &depth" not in body:
    raise SystemExit("Label insensitive pixmap lookup guard missing")
lookup = body.index("if (LabelGetPixmapData(XtScreen(lw), pix_use")
following = body[lookup:]
if "if (depth == lw->core.depth)" not in following or "XSetFillStyle" not in following:
    raise SystemExit("Label insensitive draw/stipple path is incomplete")
print("Label pixmap metadata guards are present")
