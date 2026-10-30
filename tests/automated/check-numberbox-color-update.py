#!/usr/bin/env python3
"""Check NumberBox refreshes all semantic drawing GCs after color changes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult QueryGeometry", start)
body = source[start:end]
for fragment in (
    "Boolean colors_changed =",
    "old->primitive.bottom_shadow_color != number->primitive.bottom_shadow_color",
    "if (number->number_box.text_gc)",
    "if (number->number_box.arrow_gc)",
    "if (number->number_box.invalid_gc)",
    "XSetBackground(display, number->number_box.text_gc",
    "XSetBackground(display, number->number_box.arrow_gc",
    "XSetBackground(display, number->number_box.invalid_gc",
    "redraw_needed = True;",
):
    if fragment not in body:
        raise SystemExit(f"NumberBox color update contract missing: {fragment}")

print("NumberBox color changes refresh text, arrow, and invalid-state GCs")
