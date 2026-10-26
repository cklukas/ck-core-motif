#!/usr/bin/env python3
"""Check SetValues picture-source duplication preserves ownership on failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("if (XmDataField_picture_source(old_tf)")
end = source.index("if(XmDataField_picture(new_tf))", start)
context = source[start:end]
required = (
    "new_picture_source = XmDataField_picture_source(new_tf) != NULL",
    "if (XmDataField_picture_source(new_tf) != NULL &&",
    "new_picture_source == NULL)",
    "XmDataField_picture_source(new_tf) =",
    "XtFree((char*)XmDataField_picture_source(old_tf));",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("picture-source ownership guard missing: " + ", ".join(missing))
if context.index("new_picture_source =") > context.index("XtFree((char*)XmDataField_picture_source(old_tf));"):
    raise SystemExit("old picture source is freed before duplication")

print("DataField picture-source checks passed")
