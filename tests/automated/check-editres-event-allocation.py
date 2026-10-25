#!/usr/bin/env python3
"""Check that Editres event and SetValues payload allocations are guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(encoding="utf-8")
start = source.index("static EditresEvent *\nBuildEvent")
end = source.index("static void", start)
region = source[start:end]
assert "event = (EditresEvent *) XtCalloc(sizeof(EditresEvent), 1);" in region
assert "if (event == NULL)" in region
assert "sv_event->value = XtMalloc" in region
assert "if (sv_event->value == NULL)" in region
assert "goto done;" in region
print("PASS Editres event allocations are guarded")
