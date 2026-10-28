#!/usr/bin/env python3
"""Check that Editres error-message sizing does not narrow strlen totals."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(encoding="utf-8")
start = source.index("HandleToolkitErrors(name, type, class, msg, params, num_params)")
end = source.index("/*\tFunction Name: ExecuteSetValues", start)
region = source[start:end]

assert "size_t len;" in region
assert "len <= (size_t)UINT_MAX" in region
assert "XtMalloc((Cardinal)len)" in region
assert "int len;" not in region
print("PASS Editres error-message sizing is width-safe")
