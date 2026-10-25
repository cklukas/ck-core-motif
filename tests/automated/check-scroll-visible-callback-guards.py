#!/usr/bin/env python3
"""Check scroll-visible callbacks validate callback and workspace state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "if (call_data == NULL || ACTIVE_WS == NULL" in text
assert "ACTIVE_WS->pIconBox->scrolledWidget == NULL" in text
assert "vis_data->traversal_destination == NULL" in text
print("scroll-visible callback validates callback and workspace state")
