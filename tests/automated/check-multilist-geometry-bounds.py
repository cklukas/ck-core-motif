#!/usr/bin/env python3
"""Check MultiList geometry requests avoid unsigned wraparound."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "MultiList.c"
text = source.read_text(encoding="utf-8")
assert "static Dimension multilist_frame_dimension(Dimension child, Dimension border);" in text
assert "multilist_frame_dimension(" in text
assert "uint64_t total = (uint64_t)child + 2 * (uint64_t)border;" in text
assert "total > USHRT_MAX" in text
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
