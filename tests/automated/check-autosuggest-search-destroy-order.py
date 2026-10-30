#!/usr/bin/env python3
"""Check AutoSuggestBox observes SearchBox destruction before parent teardown."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c").read_text(
    encoding="utf-8"
)
for fragment in (
    "static void search_destroyed(Widget, XtPointer, XtPointer);",
    "static void search_destroyed(Widget search, XtPointer closure, XtPointer data)",
    "box->auto_suggest.search = NULL;",
    "XtNdestroyCallback,\n                      search_destroyed, box",
    "XtRemoveCallback(box->auto_suggest.search, XtNdestroyCallback",
):
    if fragment not in source:
        raise SystemExit(f"AutoSuggestBox destroy-order contract missing: {fragment}")
if "!((CoreWidget)box->auto_suggest.search)->core.being_destroyed" in source:
    raise SystemExit("AutoSuggestBox must not inspect a potentially stale SearchBox pointer")

print("AutoSuggestBox observes SearchBox destruction safely")
