#!/usr/bin/env python3
"""Check bounds and input validation for MWM confirmation dialogs."""

from pathlib import Path


source = Path("clients/mwm/WmFeedback.c").read_text(encoding="utf-8")
start = source.index("void ConfirmAction")
end = source.index("END OF FUNCTION ConfirmAction", start)
body = source[start:end]

assert "pSD == NULL" in body
assert "nbr < 0 || nbr >= 4" in body
assert "screenTopLevelW == NULL" in body
assert "return;" in body
assert body.index("nbr < 0") < body.index("pSD->confirmboxW[nbr]")

print("PASS MWM confirmation actions are bounded")
