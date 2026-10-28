#!/usr/bin/env python3
"""Check CommandPalette invocation validates the selected-position array."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text()
start = source.index("static void invoke_callback")
end = source.index("\nWidget XmCreateCommandPalette", start)
body = source[start:end]

assert "positions == NULL" in body
assert "count < 1 || positions == NULL || positions[0] < 1" in body

print("CommandPalette invocation guards missing selected positions")
