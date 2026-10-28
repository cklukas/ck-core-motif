#!/usr/bin/env python3
"""Check DataField insertion callbacks reject incomplete selection state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataFSel.c").read_text()
start = source.index("InsertSelection(\n        Widget")
body = source[start:source.index("/* ARGSUSED */", start + 1)]

assert "w == NULL" in body
assert "insert_select == NULL" in body
assert "type == NULL" in body
assert "length == NULL" in body
assert "format == NULL" in body
assert "*format != 8" in body
assert "*length != 0 && *type == None" in body
guard = body[body.index("if (w == NULL"):body.index("if (!value)")]
assert "XtFree((char *)value);" in guard
assert body.index("if (w == NULL") < body.index("XtDisplay(w)")
empty = body[body.index("if (!value)"):body.index("if (insert_select->select_type")]
assert "if (*length == 0)" in empty
assert empty.index("if (*length == 0)") < empty.index("*(char *) value")
assert "converted = mbstowcs(wc_value, temp" in body
assert "size_t converted;" in body
assert body.count("converted == (size_t)-1") >= 2
assert body.count("num_chars = (int)converted") >= 2

targets_start = source.index("HandleInsertTargets(\n        Widget")
targets = source[targets_start:source.index("/*\n * Converts requested target", targets_start)]
assert "w == NULL" in targets
assert "insert_select == NULL" in targets
assert "seltype == NULL" in targets
assert "length == NULL" in targets
assert "insert_select->event == NULL" in targets
assert "*length != 0 && value == NULL" in targets
assert "unsigned long i;" in targets
assert targets.count("XtFree((char *)value);") >= 2
assert targets.index("XtFree((char *)value);", targets.index("for (i")) < targets.index("XtGetSelectionValue")

print("PASS DataField insertion callback validates selection inputs")
