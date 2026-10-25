#!/usr/bin/env python3
"""Keep legacy Text line-table reflow allocation failure-safe."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Text.c").read_text()
start = source.index("\nRefigureLines(XmTextWidget tw)\n")
end = source.index("\n}\n", start)
region = source[start:end]

assert "#include <stdint.h>" in source
assert "oldNumLines >= INT_MAX" in region
assert "SIZE_MAX / sizeof(LineRec)" in region
assert "UINT_MAX / sizeof(LineRec)" in region
assert "tw->text.needs_refigure_lines = TRUE" in region
assert "tw->text.in_refigure_lines = FALSE" in region
print("PASS Text line-table reflow allocation guard")
