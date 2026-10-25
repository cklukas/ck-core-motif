#!/usr/bin/env python3
"""Check accessibility D-Bus property signatures are null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c").read_text()
start = source.index("static void property_value(")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "if (!iface || !name)" in region
assert "signature = property_signature(iface, name);" in region
assert 'signature && !strcmp(signature, "d")' in region
assert 'signature && !strcmp(signature, "s")' in region
assert region.count("property_signature(iface, name)") == 1
print("PASS accessibility D-Bus property input validation")
