#!/usr/bin/env python3
"""Check Notebook layout scans guard malformed composite children."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text(encoding="utf-8")
for name, marker in (("LayoutPages (", "/* resize pages & status */"),
                    ("LayoutMajorTabs (", "total_major_tabs = 0;"),
                    ("LayoutMinorTabs (", "status = TAB_DRAW;")):
    start = source.index(name)
    end = source.find("\n/*-", start + 8)
    region = source[start:end if end != -1 else len(source)]
    assert marker in region
    assert "child == NULL || child->core.constraints == NULL" in region
print("PASS Notebook layout scans guard malformed child constraints")
