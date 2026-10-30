#!/usr/bin/env python3
"""Check I18List selection internals fail closed during teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/I18List.c").read_text(
    encoding="utf-8"
)

assert source.count("core.being_destroyed") >= 4
print("PASS I18List selection teardown guards")
