#!/usr/bin/env python3
"""Check manager child traversal uses the full Xt count width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Manager.c").read_text(
    encoding="utf-8"
)
start = source.index("CheckRemoveMotionHandlers(\n        XmManagerWidget mw")
start = source.index("{", start)
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "register Cardinal i;" in region
assert "register int i;" not in region
assert "i < mw->composite.num_children" in region
print("Manager child traversal uses a full-width Cardinal index")
