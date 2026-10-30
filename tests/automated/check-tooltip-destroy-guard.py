#!/usr/bin/env python3
"""Check optional tooltip label teardown validates lifecycle state."""

from pathlib import Path


source = Path("lib/Xm/ToolTip.c").read_text(encoding="utf-8")
start = source.rindex("ToolTipLabelDestroyCallback(Widget w")
end = source.index("\n}", start)
body = source[start:end]
assert "if (w == NULL)" in body
assert body.index("if (w == NULL)") < body.index("XmeTraitGet(w")
assert "if (shell != NULL)" in body

print("Tooltip label teardown guards present")
