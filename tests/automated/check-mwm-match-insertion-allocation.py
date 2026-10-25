#!/usr/bin/env python3
"""Check that MWM match-list insertion handles item allocation failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("static void PerformInsertionsOnMatchList")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (newMenuItem == (MenuItem *)NULL)" in region
assert "MakeClientCommandMenuItem\n\t      (curmatch->treenode->defaultName, curmatch->funcargs);" in region
assert "XtNewString(curmatch->treenode->defaultName)" not in region
assert "XtNewString(curmatch->funcargs)" not in region
print("PASS MWM match-list insertion handles allocation failure")
