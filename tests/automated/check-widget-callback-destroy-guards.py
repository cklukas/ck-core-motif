#!/usr/bin/env python3
"""Check the remaining public widget callbacks fail closed during teardown."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
checks = {
    "lib/Xm/BreadcrumbBar.c": "bar->core.being_destroyed",
    "lib/Xm/NumberBox.c": "number->core.being_destroyed",
    "lib/Xm/EmptyState.c": "widget->core.being_destroyed",
    "lib/Xm/ProgressRing.c": "ring->core.being_destroyed",
    "lib/Xm/Sidebar.c": "sidebar->core.being_destroyed",
    "lib/Xm/RowActions.c": "widget->core.being_destroyed",
    "lib/Xm/Calendar.c": "calendar->core.being_destroyed",
    "lib/Xm/TeachingTip.c": "widget->core.being_destroyed",
    "lib/Xm/Flyout.c": "widget->core.being_destroyed",
}

for relative, marker in checks.items():
    source = (ROOT / relative).read_text(encoding="utf-8")
    assert marker in source, f"missing callback teardown guard in {relative}"

print("PASS widget callbacks guard destruction")
