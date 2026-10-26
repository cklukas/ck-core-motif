#!/usr/bin/env python3
"""Ensure Text class translation storage is bounded and failure-safe."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nClassPartInitialize(WidgetClass wc)") + 1
end = text.index("\n/****************************************************************\n *\n * ClassInitialize", start)
block = text[start:end]

assert "event_bindings_len" in block
assert "SIZE_MAX - strlen(_XmTextEventBindings2)" in block
assert "SIZE_MAX - strlen(_XmTextEventBindings3) - 1" in block
assert "event_bindings_len > UINT_MAX" in block
assert "event_bindings = (char *)XtMalloc((Cardinal)event_bindings_len);" in block
assert "if (event_bindings == NULL)" in block
assert block.index("_XmFastSubclassInit") < block.index("event_bindings =")
assert block.index("if (event_bindings == NULL)") < block.index("strcpy(event_bindings")

print("text class translation allocation guard: ok")
