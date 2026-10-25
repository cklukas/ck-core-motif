#!/usr/bin/env python3
"""Check MultiList geometry requests avoid unsigned wraparound."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "MultiList.c"
text = source.read_text(encoding="utf-8")
start = text.index("if ((w == XmMultiList_frame(elist))")
end = text.index("answer = XtMakeGeometryRequest", start)
body = text[start:end]
required = [
    "int width = (int)elist->core.width -",
    "(int)w->core.width - (int)request->width",
    "ext_request.width = (Dimension)(width > 0 ? width : 1);",
    "int height = (int)elist->core.height -",
    "(int)w->core.height - (int)request->height",
    "ext_request.height = (Dimension)(height > 0 ? height : 1);",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("MultiList geometry guard missing: " + ", ".join(missing))
print("MultiList geometry requests use signed bounded dimensions")
