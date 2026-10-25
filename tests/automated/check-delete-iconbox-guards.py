#!/usr/bin/env python3
"""Check icon-box deletion validates state before teardown."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "void DeleteIconFromBox (IconBoxData *pIBD, ClientData *pCD)" in text
assert "pIBD == NULL || pCD == NULL" in text
assert "pIBD->clipWidget == NULL" in text
assert "pIBD->pCD_iconBox->widthInc <= 0" in text
assert "if (theChild != NULL)" in text
print("icon-box deletion validates state before teardown")
