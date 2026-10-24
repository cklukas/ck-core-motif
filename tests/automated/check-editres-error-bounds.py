#!/usr/bin/env python3
"""Check that EditRes diagnostics measure formatted output safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text()
start = source.index("HandleToolkitErrors(name, type, class, msg, params, num_params)")
end = source.index("\n/*\tFunction Name: ExecuteSetValues", start)
region = source[start:end]
assert "snprintf(NULL, 0, msg, params[0])" in region
assert "snprintf(pbuf, (size_t)len, msg, params[0])" in region
assert "sprintf(pbuf" not in region
assert "sprintf(buf," not in region
print("EditRes diagnostic formatting is bounded")
