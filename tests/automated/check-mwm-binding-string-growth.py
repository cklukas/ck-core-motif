#!/usr/bin/env python3
from pathlib import Path

source = Path("clients/mwm/WmInitWs.c").read_text()
start = source.index("FixupBindingsString(")
end = source.index("Boolean\nVirtKeys4DIN", start)
region = source[start:end]
assert "size_t new_len" in region
assert "size_t suffix_len" in region
assert "size_t prefix_len" in region
assert "SIZE_MAX" in region
assert "new_ttl_len + 1 > (size_t)UINT_MAX" in region
assert "String resized" in region
assert "if (resized == NULL)" in region
assert region.index("if (resized == NULL)") < region.index("fixed_str = resized")
assert "*ks_ptr = '\\0'" not in region
print("MWM binding-string growth guards present")
