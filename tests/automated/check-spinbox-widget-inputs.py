#!/usr/bin/env python3
"""Check SimpleSpinBox list APIs reject null widgets before dereferencing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SSpinB.c").read_text(
    encoding="utf-8"
)

for function in (
    "XmSimpleSpinBoxAddItem(",
    "XmSimpleSpinBoxDeletePos(",
    "XmSimpleSpinBoxSetItem(",
):
    start = source.index(function)
    body_start = source.index("{", start)
    body_end = source.index("\n}", body_start)
    body = source[body_start:body_end]
    guard = "if (ssb_w == (Widget) NULL)\n\treturn;"
    assert guard in body
    assert body.index(guard) < body.index("SSB_SIMPLE_SPIN_BOX_PART(ssb_w)")

print("SimpleSpinBox list APIs guard null widgets before widget-part access")
