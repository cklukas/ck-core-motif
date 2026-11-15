#!/usr/bin/env python3
"""Check that ProgressBar geometry queries preserve explicit dimensions."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ProgressBar.c"
text = source.read_text(encoding="utf-8")
start = text.index("static XtGeometryResult QueryGeometry(Widget widget")
end = text.index("\nstatic void Notify", start)
query = text[start:end]

assert "preferred_size(progress,&reply->width,&reply->height);" in query
assert "if (widget->core.width)" in query
assert "reply->width = widget->core.width;" in query
assert "if (widget->core.height)" in query
assert "reply->height = widget->core.height;" in query
assert "return XmeReplyToQueryGeometry(widget, request, reply);" in query
assert "(void)request;" not in query
assert "if (fill_width > (Dimension)bar_width)" in text
assert "? bar_x + bar_width - (int)fill_width : bar_x;" in text

print("ProgressBar QueryGeometry preserves explicit dimensions")
