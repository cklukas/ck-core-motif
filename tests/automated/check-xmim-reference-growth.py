#!/usr/bin/env python3
"""Check XIM reference-table growth is atomic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.index("static Cardinal\nadd_ref(")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "Widget *new_refs;" in region
assert "XIMCallback **new_callbacks;" in region
assert "if (new_refs == NULL || new_callbacks == NULL)" in region
assert "return 0;" in region
assert region.index("if (new_refs == NULL") < region.index("refs->refs = new_refs")
assert "memcpy(new_refs" in region and "memcpy(new_callbacks" in region
print("PASS XmIm reference-table growth")
