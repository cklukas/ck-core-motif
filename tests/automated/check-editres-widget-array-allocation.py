#!/usr/bin/env python3
"""Check that Editres widget-array allocations are guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(encoding="utf-8")
start = source.index("static EditresEvent *\nBuildEvent")
end = source.index("static void", start)
region = source[start:end]

assert "if (sv_event->widgets == NULL && sv_event->num_entries != 0)" in region
assert "if (find_event->widgets == NULL)" in region
assert "if (get_event->widgets == NULL && get_event->num_entries != 0)" in region
assert "gv_event->num_entries == 0" in region
assert "if (gv_event->widgets == NULL)" in region
assert "if (!_XEditResGetWidgetInfo(stream, gv_event->widgets))" in region
print("PASS Editres widget-array allocations are guarded")
