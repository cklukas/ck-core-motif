#!/usr/bin/env python3
"""Check icon-box focus callbacks validate widget and event inputs."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "void ChangeActiveIconboxIconText" in text
assert "if (icon == NULL || event == NULL)" in text
print("icon-box focus callback validates widget and event inputs")
