#!/usr/bin/env python3
"""Check value/action callbacks fail closed during widget destruction."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
checks = {
    "lib/Xm/Stepper.c": "s == NULL || s->core.being_destroyed",
    "lib/Xm/ProgressBar.c": "progress == NULL || progress->core.being_destroyed",
    "lib/Xm/Spinner.c": "spinner == NULL || spinner->core.being_destroyed",
    "lib/Xm/InfoBar.c": "!XmIsInfoBar(widget)||widget->core.being_destroyed",
}

for relative, marker in checks.items():
    source = (ROOT / relative).read_text(encoding="utf-8")
    assert marker in source, f"missing destruction guard in {relative}"

print("PASS value and action callbacks guard widget destruction")
