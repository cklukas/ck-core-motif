#!/usr/bin/env python3
"""Check that WSM withdrawal skips detached workspace records safely."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmManage.c"
text = source.read_text()
assert text.count("if (pCD->pSD != NULL && pCD->pWsList != NULL)") >= 2
assert text.count("if (pWS != NULL)") >= 2
assert text.count("TakeClientOutOfWorkspace (pWS, pCD);") >= 2
print("withdrawal workspace cleanup validates list and workspace records")
