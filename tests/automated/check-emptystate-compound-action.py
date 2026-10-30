#!/usr/bin/env python3
"""Check EmptyState tests action labels without flattening compound strings."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EmptyState.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean has_action")
end = source.index("static void preferred", start)
body = source[start:end]
if "XmStringEmpty(state->empty_state.action_label)" not in body:
    raise SystemExit("EmptyState must use XmStringEmpty for action visibility")
if "XmStringUnparse" in body:
    raise SystemExit("EmptyState action visibility must not flatten compound strings")

print("EmptyState preserves compound action labels when testing visibility")
