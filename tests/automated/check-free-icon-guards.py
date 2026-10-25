#!/usr/bin/env python3
"""Check that FreeIcon validates teardown state before dereferencing it."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmManage.c"
text = source.read_text()

prologue = text.index("void FreeIcon (ClientData *pCD)")
body = text[prologue:text.index("} /* END OF FUNCTION FreeIcon */", prologue)]
assert "if (pCD == NULL || pCD->pSD == NULL)" in body
assert "pCD->numInhabited > 0" in body
assert "pCD->pWsList != NULL" in body
assert "pCD->pWsList[0].pIconBox" in body
print("FreeIcon validates client, screen, and workspace state")
