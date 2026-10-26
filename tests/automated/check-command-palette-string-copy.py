#!/usr/bin/env python3
"""Ensure CommandPalette retains compound-string item data."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/CommandPalette.c").read_text()
required = 'return value ? XmStringCopy(value) : XmStringCreateLocalized("");'
if required not in source:
    raise SystemExit("CommandPalette item strings are still flattened")

print("PASS CommandPalette preserves compound item strings")
