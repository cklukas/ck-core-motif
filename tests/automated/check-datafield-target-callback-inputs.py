#!/usr/bin/env python3
"""Check DataField target callbacks validate state before display access."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_HandleTargets(\n        Widget")
end = source.index("  /*\n   * Set stuff position", start)
body = source[start:end]

assert "w == NULL" in body
assert "tmp_action == NULL" in body
assert "tmp_action->event == NULL" in body
assert "seltype == NULL" in body
assert "length == NULL" in body
assert "*length != 0 && value == NULL" in body
assert "unsigned long i;" in body
assert body.index("if (w == NULL") < body.index("XtDisplay(w)")
assert "XtFree((char *)value);" in body
request = source[source.index("XtGetSelectionValue(w, XA_PRIMARY, targets[0], df_DoStuff"):]
assert "prim_select->time);" in request

print("PASS DataField target callback validates selection inputs")
