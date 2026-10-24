#!/usr/bin/env python3
"""Check that Motif warning formatting is dynamically bounded."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/VendorS.c").read_text()
start = source.index("MotifWarningHandler (String name")
end = source.index("\n}\n", start) + 3
body = source[start:end]
assert "snprintf(NULL, 0" in body
assert "XtMalloc((Cardinal)(combined_len + 1))" in body
assert "XtWarning(output);" in body
assert "sprintf(buf" not in body
assert "strcpy (&buf2" not in body
print("Motif warning-buffer bounds contract passed")
