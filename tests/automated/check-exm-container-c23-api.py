#!/usr/bin/env python3
"""Verify the unsupported Exm Container constructor declaration."""

from pathlib import Path


root = Path(__file__).resolve().parents[2] / "demos" / "unsupported" / "Exm"
header = (root / "Container.h").read_text(encoding="utf-8")
source = (root / "Container.c").read_text(encoding="utf-8")

assert "extern Widget ExmCreateContainer(Widget, char *, ArgList, Cardinal);" in header
assert "extern Widget ExmCreateContainer();" not in header
assert "ExmCreateContainer (\n        Widget parent," in source
assert "ArgList arglist" in source
assert "Cardinal argcount" in source

print("Exm Container constructor declaration matches its C23 definition")
