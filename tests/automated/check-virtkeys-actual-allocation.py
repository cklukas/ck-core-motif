#!/usr/bin/env python3
"""Check virtual-key result-array allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text()
start = source.index("XmeVirtualToActualKeysyms(")
end = source.index("\nBoolean \n_XmVirtKeysLoadFileBindings", start)
region = source[start:end]

assert "int           match_count;" in region
assert "if (actualKeyData == NULL)" in region
assert "match_count = matches;" in region
assert "if (*actualKeyData == NULL)" in region
assert "return 0;" in region

print("PASS virtual-key result allocation checks")
