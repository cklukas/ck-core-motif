#!/usr/bin/env python3
"""Check Notebook unhighlight callback guards child constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text(encoding="utf-8")
start = source.index("GetUnhighlightGC (")
end = source.index("\n}\n", start)
region = source[start:end]
assert "if (child != NULL && child->core.constraints != NULL)" in region
assert region.index("child->core.constraints != NULL") < region.index("nc = NotebookConstraint(child)")
print("PASS Notebook unhighlight callback guards malformed child constraints")
