#!/usr/bin/env python3
"""Check ComboBox convenience mutators fail closed during Xt teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/ComboBox.c").read_text(
    encoding="utf-8"
)

assert source.count("else if (cb->core.being_destroyed)") >= 5
assert source.count("_XmAppUnlock(app);\n      return;") >= 5
print("PASS ComboBox mutators guard destruction")
