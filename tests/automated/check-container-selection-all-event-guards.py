#!/usr/bin/env python3
"""Check Container select-all actions reject missing events."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
select_start = source.index("ContainerSelectAll(\n        Widget")
deselect_start = source.index("ContainerDeselectAll(\n        Widget", select_start)
activate_start = source.index("ContainerActivate(\n        Widget", deselect_start)
select = source[select_start:deselect_start]
deselect = source[deselect_start:activate_start]
assert "if (event == NULL)" in select
assert "if (event == NULL)" in deselect
assert select.index("if (event == NULL)") < select.index("event->xbutton")
assert deselect.index("if (event == NULL)") < deselect.index("event->xbutton")

print("PASS Container select-all actions check their events")
