#!/usr/bin/env python3
"""Check DataField GC creation and context use are failure-safe."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("if (XmTextF_has_rect(tf))")
end = source.index("static void \n#ifdef _NO_PROTO\ndf_MakeIBeamOffArea", start)
context = source[start:end]
required = (
    "if (gc_data == NULL)",
    "if (XmTextF_save_gc(tf) == NULL)",
    "if (XmTextF_gc(tf) == NULL)",
    "if (XmTextF_image_gc(tf) == NULL)",
    "XmTextF_save_gc(tf) = NULL;",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("GC-resource guard missing: " + ", ".join(missing))

print("DataField GC-resource checks passed")
