#!/usr/bin/env python3
"""Check TextIn secondary allocations fail without null dereferences."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text()
context = source[source.index("GetTextDestData("):source.index("Boolean\n_XmTextNeedsPendingDeleteDis")]
assert "if (ctx_data == NULL)" in context
assert "if (dest_data == NULL)" in context
assert "XtFree((char *)ctx_data);" in context

action = source[source.index("ProcessBSelect("):source.index("/* This function make the request", source.index("ProcessBSelect("))]
assert "if (data->transfer_action == NULL)" in action
assert "if (data->transfer_action == NULL)\n\t    return;" in action
assert "if (data->transfer_action->event == NULL)" in action
assert "data->transfer_action = NULL;" in action

stuff_start = source.index("Stuff(", source.index("Stuff(") + 1)
stuff = source[stuff_start:source.index("\n}\n", stuff_start)]
assert "if (point == NULL)" in stuff
print("PASS TextIn secondary allocations are failure-safe")
