#!/usr/bin/env python3
"""Check MWM key-substitution table growth and token widths."""
from pathlib import Path

text = (Path(__file__).parents[2] / "clients/mwm/WmResParse.c").read_text()
start = text.index("static void InitKeySubs")
end = text.index("#endif /* WSM */", start)
body = text[start:end]
assert "numKS == INT_MAX" in body
assert "UINT_MAX / sizeof(KeySub)" in body
assert "len = (size_t)(pch1 - pch0);" in body
assert "if (len < 1 || len > INT_MAX)" in body
assert "pKS->lenFrom = (int)len;" in body
to_start = body.index("/* get \"to\" string */")
to_body = body[to_start:]
assert "len = (size_t)(pch1 - pch0);" in to_body
assert "if (len < 1 || len >= UINT_MAX)" in to_body
assert "XtFree((char *)pKS->pchFrom);" in to_body
assert "pKS->pchFrom = NULL;" in to_body
print("PASS MWM key-substitution parsing bounds table and token widths")
