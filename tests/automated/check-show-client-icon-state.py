#!/usr/bin/env python3
"""Check icon-state updates reject a missing client."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "void ShowClientIconState (ClientData *pCD, int newState)" in text
assert "if (pCD == NULL)" in text
print("icon-state updates validate the client pointer")
