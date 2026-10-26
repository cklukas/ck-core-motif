#!/usr/bin/env python3
"""Check initial picture-source duplication before ownership transfer."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("String picture_source = XtNewString")
end = source.index("} else {", start)
context = source[start:end]
required = (
    "if (picture_source != NULL)",
    "XmDataField_picture_source(tf) = picture_source;",
    "XmDataField_picture(tf) = XmParsePicture(picture_source);",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("picture initialization guard missing: " + ", ".join(missing))

print("DataField picture-init checks passed")
