#!/usr/bin/env python3
"""Check that MWM validates backdrop pixmap request IDs."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmEvent.c").read_text()
start = source.index("DTWM_REQ_CHANGE_BACKDROP")
end = source.index("\n\t    if (sRequest)", start)
region = source[start:end]

assert "parsed = strtoul(pch, &end, 16);" in region
assert "pch[0] != '-' && pch[0] != '+'" in region
assert "end != pch &&" in region
assert "*end == '\\0'" in region
assert "errno != ERANGE" in region
assert "pixmap = (Pixmap)parsed;" in region
assert 'Warning ("Invalid pixmap id for backdrop change request.");' in region
assert "sscanf (pch, \"%lx\"" not in region
print("MWM backdrop pixmap contract passed")
