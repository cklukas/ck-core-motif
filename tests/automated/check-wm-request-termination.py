#!/usr/bin/env python3
"""Check safe extraction of unterminated _DT_WM_REQUEST properties."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProperty.c").read_text(
    encoding="utf-8"
)
start = source.index("GetDtWmRequest (", source.index("GetDtWmRequest ("))
end = source.index("GetIntialPropertyList (", start)
section = source[start:end]
assert "size_t requestLength;" in section
assert "memcpy(chRequest, &property[iNext], requestLength - 1);" in section
assert "chRequest[requestLength - 1] = '\\0';" in section
assert "property [i] == '\\0'" in section
assert "strncpy (chRequest" not in section
assert "property [i] != '\\0'" not in section
print("Mwm window-management requests are NUL-terminated without over-read")
