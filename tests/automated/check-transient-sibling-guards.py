#!/usr/bin/env python3
"""Check sibling reordering handles malformed transient chains."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
for name in ("PutTransientOnTop", "PutTransientBelowSiblings"):
    start = text.index(f"Boolean {name}")
    end = text.index("} /* END OF FUNCTION", start)
    body = text[start:end]
    assert "if (pcd == NULL)" in body
    assert "return (False);" in body
on_top = text[text.index("Boolean PutTransientOnTop"):text.index("Boolean PutTransientBelowSiblings")]
assert "pcdPrev != NULL" in on_top
below = text[text.index("Boolean PutTransientBelowSiblings"):text.index("/*************************************<->", text.index("Boolean PutTransientBelowSiblings") + 10)]
assert "pcdNext == NULL" in below
print("transient sibling reordering validates chain state")
