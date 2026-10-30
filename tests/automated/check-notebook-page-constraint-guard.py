#!/usr/bin/env python3
"""Check page-info lookup skips malformed child constraint storage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text(encoding="utf-8")
start = source.index("XmNotebookGetPageInfo (")
end = source.index("XmCreateNotebook(", start)
region = source[start:end]
assert "if (child == NULL || child->core.constraints == NULL)" in region
assert region.index("child->core.constraints == NULL") < region.index("nc = NotebookConstraint(child)")
print("PASS Notebook page-info lookup guards malformed child constraints")
