#!/usr/bin/env python3
"""Ensure a disabled Disclosure cannot transition through its custom input."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Disclosure.c").read_text(
    encoding="utf-8"
)
for marker in ("static void activate(", "static void key("):
    start = source.index(marker)
    end = source.index("\n}\n", start) + 3
    body = source[start:end]
    guard = "!XtIsSensitive((Widget)w)"
    if guard not in body:
        raise SystemExit(f"{marker} lacks disabled Disclosure guard")
    if body.index(guard) > body.index("w->disclosure.expanded"):
        raise SystemExit(f"{marker} changes state before its disabled guard")

for filename, marker, receiver in (
    ("EmptyState.c", "static void activate(Widget widget,", "state"),
    ("Toolbar.c", "static void activate(Widget child,", "toolbar"),
):
    text = (Path(__file__).parents[2] / "lib" / "Xm" / filename).read_text(
        encoding="utf-8"
    )
    start = text.index(marker)
    end = text.index("\n}\n", start) + 3
    body = text[start:end]
    guard = f"!XtIsSensitive((Widget){receiver})"
    if guard not in body:
        raise SystemExit(f"{filename} action callback lacks disabled guard")
    if body.index(guard) > body.index("XtCallCallbacks"):
        raise SystemExit(f"{filename} dispatches before its disabled guard")

print("PASS composite controls ignore user actions while disabled")
