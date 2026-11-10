#!/usr/bin/env python3
"""Check that an empty workspace-hint list is a successful empty result."""

from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" /
          "WmProperty.c").read_text(encoding="utf-8")
start = source.index("Status GetWorkspaceHints")
end = source.index("SetWorkspaceListProperty", start)
function = source[start:end]

allocation = "if (pWsHints->numWorkspaces != 0)"
assert allocation in function
assert function.index(allocation) < function.index("XtMalloc", function.index(allocation))
assert "*pCount = pWsHints->numWorkspaces;" in function
assert "*ppWsAtoms = paWs;" in function

print("PASS empty workspace hints avoid a zero-byte allocation")
