#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"Notebook.c").read_text()
assert "XmNotebookGetPageInfo (" in text
assert "if (notebook == NULL || page_info == NULL)" in text
assert "return XmPAGE_INVALID;" in text
print("XmNotebook page lookup validates widget and output")
