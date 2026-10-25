#!/usr/bin/env python3
"""Check that accessibility menu snapshots handle allocation failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void menu_changed(")
end = text.index("\nstatic void node_destroyed", start)
region = text[start:end]
assert "XmAccessibleId *ids = count" in region
assert "if (count != 0 && !ids)" in region
assert "return;" in region
print("accessibility menu snapshots guard allocation")
