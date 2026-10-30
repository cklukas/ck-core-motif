#!/usr/bin/env python3
"""Check Container button-two actions reject missing parameter counts."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
down_start = source.index("ContainerHandleBtn2Down(\n        Widget")
motion_start = source.index("ContainerHandleBtn2Motion(\n        Widget")
up_start = source.index("ContainerHandleBtn2Up(\n        Widget")
down = source[down_start:motion_start]
motion = source[motion_start:up_start]
assert "if (num_params == NULL || *num_params < 2)" in down
assert "if (num_params == NULL || *num_params < 1)" in motion

print("PASS Container button-two actions check parameter counts")
