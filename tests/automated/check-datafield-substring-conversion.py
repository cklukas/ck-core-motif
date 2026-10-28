#!/usr/bin/env python3
"""Check DataField substring exports detect wide conversion failures."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("XmDataFieldGetSubstring(\n\tWidget")
end = source.index("\nXmTextPosition \n#ifdef", start)
body = source[start:end]
assert "size_t wcs_ret = 0;" in body
assert "wcs_ret == (size_t)-1" in body

wide_start = source.index("XmDataFieldGetSubstringWcs(\n\tWidget")
wide_body = source[wide_start:source.index("\nXmTextPosition \n#ifdef", wide_start)]
assert "size_t num_wcs = 0;" in wide_body
assert "num_wcs == (size_t)-1" in wide_body

print("PASS DataField substring exports detect conversion failure")
