#!/usr/bin/env python3
"""Check icon-box keypress callbacks validate widget and event inputs."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "void HandleIconBoxIconKeyPress" in text
assert "if (icon == NULL || keyEvent == NULL)" in text
print("icon-box keypress callback validates widget and event inputs")
