#!/usr/bin/env python3
"""Check TextField TARGETS conversion handles target-list allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("if (*target == atoms[XmATARGETS])")
end = source.index("} else if (*target == atoms[XmATIMESTAMP])", start)
branch = source[start:end]

assert "Atom *targs = XmeStandardTargets(w, 10, &target_count);" in branch
assert "if (targs == NULL)" in branch
assert "return False;" in branch

print("TextField TARGETS export checks target-list allocation")
