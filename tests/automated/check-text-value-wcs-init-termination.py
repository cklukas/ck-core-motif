#!/usr/bin/env python3
"""Check XmText valueWcs initialization conversion bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Text.c").read_text()
start = source.index("static void \nInitializeHook(Widget wid,")
end = source.index("\nstatic void ", start)
body = source[start:end]

assert "n_bytes != (size_t)-1 && n_bytes < allocation_bytes" in body
assert "tmp_value[n_bytes] = 0;" in body
assert "_XmStringSourceSetValue(tw, tmp_value);" in body

print("XmText valueWcs initialization checks conversion termination")
