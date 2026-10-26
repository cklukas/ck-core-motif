#!/usr/bin/env python3
"""Ensure Text on-the-spot state initialization is allocation-safe."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("newtw->text.onthespot =")
end = text.index("newtw->text.tm_table", start)
block = text[start:end]

assert "XtMalloc(sizeof(OnTheSpotDataRecTW)" in block
assert "if (newtw->text.onthespot != NULL)" in block
assert "newtw->text.onthespot->start" in block
assert "newtw->text.onthespot->verify_commit = False;" in block

print("text on-the-spot allocation guard: ok")
