#!/usr/bin/env python3
"""Check that Mwm feedback text formatting is bounded and type-correct."""
from pathlib import Path

root = Path(__file__).parents[2]
global_source = (root / "clients" / "mwm" / "WmGlobal.h").read_text(encoding="utf-8")
feedback_source = (root / "clients" / "mwm" / "WmFeedback.c").read_text(
    encoding="utf-8"
)
assert "char fbLocation[32];" in global_source
assert "char fbSize[32];" in global_source
assert "snprintf (pSD->fbLocation, sizeof pSD->fbLocation" in feedback_source
assert "snprintf (pSD->fbSize, sizeof pSD->fbSize" in feedback_source
assert '"%4ux%-4u"' in feedback_source
assert 'sprintf (pSD->fbLocation' not in feedback_source
assert 'sprintf (pSD->fbSize' not in feedback_source
print("Mwm feedback text uses bounded, correctly typed buffers")
