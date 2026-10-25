#!/usr/bin/env python3
"""Check that virtual-key fallback binding allocation failure is safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text(encoding="utf-8")
start = source.index("_XmVirtKeysLoadFallbackBindings(Display")
end = source.index("/*\tFunction Name:", start) if "/*\tFunction Name:" in source[start:] else len(source)
region = source[start:end]
assert region.count("fileName != NULL && (fp = fopen") == 2
assert region.count("if (*binding != NULL)") >= 2
assert "if (*binding == NULL)" in region
assert "return 0;" in region
print("PASS virtual-key fallback allocations are guarded")
