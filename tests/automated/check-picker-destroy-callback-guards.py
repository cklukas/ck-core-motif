#!/usr/bin/env python3
"""Check picker callbacks fail closed while their widget is being destroyed."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
checks = {
    "lib/Xm/ColorPicker.c": "if (picker == NULL || picker->core.being_destroyed)",
    "lib/Xm/RatingControl.c": "rating == NULL || rating->core.being_destroyed",
    "lib/Xm/DatePicker.c": "if (picker == NULL || picker->core.being_destroyed)",
    "lib/Xm/TimePicker.c": "if (picker == NULL || picker->core.being_destroyed)",
}

for relative, marker in checks.items():
    source = (ROOT / relative).read_text(encoding="utf-8")
    assert marker in source, f"missing teardown callback guard in {relative}"

print("PASS picker callbacks guard widget destruction")
