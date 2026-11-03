#!/usr/bin/env python3
"""Check picker and popup geometry replies honor Xt request-mode bits."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
for name in ("Calendar.c", "DatePicker.c", "TimePicker.c", "TeachingTip.c"):
    source = (root / name).read_text(encoding="utf-8")
    start = source.index("static XtGeometryResult QueryGeometry(Widget widget")
    end = source.find("\n}\n", start) + 2
    query = source[start:end]
    assert "XmeReplyToQueryGeometry(widget," in query, name
    assert "return XtGeometryAlmost;" not in query, name

print("Picker QueryGeometry methods honor Xt request-mode semantics")
