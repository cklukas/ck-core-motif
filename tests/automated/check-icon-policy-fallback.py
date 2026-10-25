#!/usr/bin/env python3
"""Check icon-box policy parsing has a safe writable fallback."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "static char defaultIconBoxPolicy[] = \"all\";" in text
assert "iconBoxSBDisplayPolicy == NULL" in text
assert "pWS->pSD->iconBoxSBDisplayPolicy = defaultIconBoxPolicy;" in text
assert "strcpy(pWS->pSD->iconBoxSBDisplayPolicy, \"all\")" not in text
print("icon-box policy parsing has a safe fallback")
