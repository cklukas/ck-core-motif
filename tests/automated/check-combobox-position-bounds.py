#!/usr/bin/env python3
"""Keep ComboBox zero-based position conversion free of signed overflow."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("CBSetSelectedPos(")
end = source.index("\n}\n", start)
region = source[start:end]

assert "#include <limits.h>" in source
assert "new_pos == INT_MAX" in region
assert region.index("new_pos == INT_MAX") < region.index("new_pos++")
print("PASS ComboBox zero-based position conversion rejects INT_MAX")
