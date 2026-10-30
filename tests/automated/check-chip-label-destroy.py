#!/usr/bin/env python3
"""Check Chip releases its owned label copy during destruction."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Chip.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Destroy(Widget widget)")
end = source.index("static void label_metrics", start)
region = source[start:end]

assert "if (chip->chip.label) XmStringFree(chip->chip.label);" in region
assert "Xt owns XmNchipLabel resource storage" not in region

print("PASS Chip releases its owned label")
