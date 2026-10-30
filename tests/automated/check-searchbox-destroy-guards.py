#!/usr/bin/env python3
"""Check SearchBox mutation and callback paths fail closed during teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/SearchBox.c").read_text(
    encoding="utf-8"
)

assert source.count("core.being_destroyed") >= 7
assert "!((XmSearchBoxWidget)widget)->core.being_destroyed" in source
assert "!XmIsSearchBox(widget) || box->core.being_destroyed" in source
print("PASS SearchBox teardown guards")
