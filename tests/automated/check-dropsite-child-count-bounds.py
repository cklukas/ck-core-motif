#!/usr/bin/env python3
"""Check DropSite descendant traversal uses a full-width child index."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropSMgr.c").read_text(
    encoding="utf-8"
)
start = source.index("HasDropSiteDescendant(\n\tXmDropSiteManagerObject dsm")
start = source.index("{", start)
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "Cardinal i;" in region
assert "int i;" not in region
assert "i < cw->composite.num_children" in region
print("DropSite descendant traversal uses a full-width Cardinal index")
