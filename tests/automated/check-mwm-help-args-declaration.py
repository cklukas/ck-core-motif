#!/usr/bin/env python3
"""Check the MWM help-argument declaration matches its implementation."""

from pathlib import Path


root = Path(__file__).parents[2] / "clients/mwm"
header = (root / "WmResParse.h").read_text(encoding="utf-8")
source = (root / "WmResParse.c").read_text(encoding="utf-8")

assert "WmDtGetHelprgs" not in header
assert "WmDtGetHelpArgs(char *args," in header
assert "unsigned char *volume" in header
assert "unsigned char *topic" in header
assert "WmDtGetHelpArgs(char *args," in source

print("PASS MWM help-argument declaration matches implementation")
