#!/usr/bin/env python3
"""Ensure SegmentedControl child callbacks validate teardown inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text(
    encoding="utf-8"
)
changed_start = source.index("static void child_changed(Widget child")
key_start = source.index("static void child_key(Widget child")
changed = source[changed_start:source.index("\n}\n", changed_start)]
key = source[key_start:source.index("\n}\n", key_start)]
assert "if (control == NULL || child == NULL)" in changed
assert "if (control == NULL || child == NULL || event == NULL)" in key
print("SegmentedControl child callbacks validate teardown inputs")
