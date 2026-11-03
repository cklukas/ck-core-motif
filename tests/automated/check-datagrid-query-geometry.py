#!/usr/bin/env python3
"""Check that DataGrid geometry queries preserve explicit allocations."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c"
text = source.read_text(encoding="utf-8")
start = text.index("static XtGeometryResult QueryGeometry(Widget widget")
end = text.index("\nWidget XmCreateDataGrid", start)
query = text[start:end]

assert "reply->width = widget->core.width ? widget->core.width : 480;" in query
assert "reply->height = widget->core.height ? widget->core.height : 240;" in query
assert "return XmeReplyToQueryGeometry(widget, request, reply);" in query
assert "(void)widget; (void)request;" not in query

print("DataGrid QueryGeometry preserves the actual allocated dimensions")
