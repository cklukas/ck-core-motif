#!/usr/bin/env python3
"""Check AutoSuggestBox mutators fail closed during Xt teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/AutoSuggestBox.c").read_text(
    encoding="utf-8"
)

assert source.count("if (box->core.being_destroyed)") >= 4
assert "XtCallCallbackList(widget, box->auto_suggest.committed_callback, &callback);\n    XtFree(id);\n    XtFree(query);\n    if (box->core.being_destroyed)" in source
print("PASS AutoSuggestBox mutators guard destruction")
