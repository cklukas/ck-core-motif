#!/usr/bin/env python3
"""Check data-field DELETE conversion guards a missing request event."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataFSel.c").read_text()
start = source.index("} else if (*target == DELETE)")
body = source[start:]

assert "req_event = XtGetSelectionRequest" in body
assert "if (req_event == NULL)" in body
assert "return False;" in body
assert "req_event->time" in body

print("Data-field DELETE conversion guards missing request events")
