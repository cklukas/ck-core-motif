#!/usr/bin/env python3
"""Check that AT-SPI memo-record allocations fail closed."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c").read_text()

assert "record = memo_get(t, id, 1);" in source
assert "if (!record)" in source
assert "TransportState *record = memo_get(t, id, 1);" in source
assert source.count("record = memo_get(t, id, 1);\n  if (!record)") >= 2
assert "record = memo_get(t, id, 1);\n    if (!record)" in source
assert "record = memo_get(t, ids[k], 1);\n    if (!record)" in source
print("PASS accessibility D-Bus memo allocation failures are guarded")
