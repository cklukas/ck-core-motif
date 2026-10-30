#!/usr/bin/env python3
"""Check XmNotebookGetPageInfo rejects non-Notebook widgets early."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text(encoding="utf-8")
start = source.index("XmNotebookGetPageInfo (")
end = source.index("XmCreateNotebook(", start)
region = source[start:end]
assert "if (notebook == NULL || !XmIsNotebook(notebook) || page_info == NULL)" in region
assert region.index("if (notebook == NULL || !XmIsNotebook(notebook) || page_info == NULL)") < region.index("_XmWidgetToAppContext(notebook)")
print("PASS XmNotebookGetPageInfo validates widget class before toolkit access")
