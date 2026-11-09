#!/usr/bin/env python3
"""Keep InfoBar geometry negotiation aligned with its current allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/InfoBar.c").read_text(encoding="utf-8")
start = source.index("static XtGeometryResult QueryGeometry")
end = source.index("void XmInfoBarDismiss", start)
body = source[start:end]

if "widget->core.width" not in body or "widget->core.height" not in body:
    raise SystemExit("InfoBar geometry query must report its actual allocation")
if "XmeReplyToQueryGeometry(widget,request,reply)" not in body:
    raise SystemExit("InfoBar geometry query must use Xt-compatible reply semantics")
if "reply->width=360" in body or "reply->height=34" in body:
    raise SystemExit("InfoBar geometry query must not advertise stale default dimensions")

print("PASS InfoBar geometry query reports the current allocation")
