#!/usr/bin/env python3
"""Check that WSM config-format debug output writes to its buffer."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/debug.c"
text = source.read_text(encoding="utf-8")

assert 'DebugAppend(str, used, "Accepts Diffs: %s\\n"' in text
assert 'sprintf("Accepts Diffs: %s\\n"' not in text
assert "static void\nDebugAppend" in text
assert "vsnprintf(str + *used, available" in text
assert "sprintf(" not in text
assert "strcat(" not in text
print("WSM config-format debug output has a valid destination")
