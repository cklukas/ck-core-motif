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
    "static Position dialog_bounded_position(long long value)",
    "x = ((long long)XtWidth(parent) - XtWidth(child)) / 2;",
    "total_width = (long long)XtWidth(child) + 2 * XtBorderWidth(child);",
    "*xRtn = dialog_bounded_position(x);",
    "*yRtn = dialog_bounded_position(y);",
):
    assert fragment in source, fragment

position_start = source.index("static void \nGetDefaultPosition")
position_end = source.index("\n\n/*", position_start)
position = source[position_start:position_end]
realized_block = """if (XtIsRealized(parent))
    {
        XtTranslateCoords(parent, dialog_bounded_position(x),
                          dialog_bounded_position(y),
                          &translated_x, &translated_y);
        x = translated_x;
        y = translated_y;
    }"""
assert realized_block in position

print("DialogShell input-method geometry and unrealized-parent placement are safe")
