#!/usr/bin/env python3
from pathlib import Path

source = Path("clients/mwm/WmInitWs.c").read_text()
start = source.index("FixupBindingsString(")
end = source.index("Boolean\nVirtKeys4DIN", start)
region = source[start:end]
assert "size_t new_len" in region
assert "size_t suffix_len" in region
assert "size_t orig_len" in region
assert "size_t prefix_len" in region
assert "(ks_ptr == fixed_str) ||" in region
assert "if (bindingsString == NULL)" in region
assert "if (fixed_str == NULL)" in region
assert "SIZE_MAX" in region
assert "new_ttl_len + 1 > (size_t)UINT_MAX" in region
assert "String resized" in region
assert "if (resized == NULL)" in region
assert region.index("if (resized == NULL)") < region.index("fixed_str = resized")
assert "*ks_ptr = '\\0'" not in region
assert "binding_length > INT_MAX" in source
assert "(int)binding_length" in source
print("MWM binding-string growth guards present")
