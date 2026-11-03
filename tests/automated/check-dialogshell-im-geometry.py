#!/usr/bin/env python3
"""Check DialogShell bounds input-method height negotiation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DialogS.c").read_text(
    encoding="utf-8"
)

for fragment in (
    "static Dimension dialog_bounded_dimension(uint64_t value)",
    "value > USHRT_MAX",
    "dialog_bounded_dimension((uint64_t)XtHeight(child) +",
    "(uint64_t)request->height + ve->vendor.im_height",
    "wid->core.height = my_request.height > ve->vendor.im_height",
):
    assert fragment in source, fragment

print("DialogShell input-method geometry stays within Dimension bounds")
