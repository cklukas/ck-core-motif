#!/usr/bin/env python3
"""Ensure Toast SetValues reports changed content and uses XmString emptiness."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Toast.c").read_text()
required = (
    "!XmStringEmpty(toast->toast.action_label)",
    "if (changed) negotiate(toast); return changed;",
)
missing = [item for item in required if item not in source]
if missing:
    raise SystemExit("Toast SetValues contract missing: " + ", ".join(missing))

print("PASS Toast reports SetValues changes and preserves compound action labels")
