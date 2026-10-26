#!/usr/bin/env python3
"""Ensure TextField class translation storage is bounded."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("ClassPartInitialize(WidgetClass w_class)")
end = text.index("\n/****************************************************************\n *\n * Private functions", start)
block = text[start:end]

assert "size_t event_bindings_len" in block
assert "SIZE_MAX - strlen(EventBindings2)" in block
assert "SIZE_MAX - strlen(EventBindings3) - 1" in block
assert "event_bindings_len + strlen(EventBindings3) + 1 > UINT_MAX" in block
assert "XtMalloc((Cardinal)event_bindings_len)" in block
assert "if (event_bindings == NULL)" in block
assert block.index("_XmFastSubclassInit") < block.index("event_bindings =")

print("TextField class translation allocation guard: ok")
