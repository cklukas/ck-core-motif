#!/usr/bin/env python3
"""Check that Mwm menu warnings remain visible and bounded for long names."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(
    encoding="utf-8"
)
assert "char pch[MAXWMPATH + 1];" in source
assert "snprintf(pch, sizeof pch, format, message);" in source
assert "if (strlen(format) + strlen(message)" not in source
assert "sprintf (pch, format, message);" not in source
print("Mwm menu warnings use bounded formatting without silent suppression")
