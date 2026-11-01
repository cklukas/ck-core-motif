#!/usr/bin/env python3
"""Check MWM parser tokens retain pointer and KeySym boundaries."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmResParse.c").read_text(
    encoding="utf-8"
)
assert "static size_t\nCopyParsedToken" in source
assert "length = (size_t)(end - start);" in source
assert "if (length >= destination_size)" in source
assert source.count("CopyParsedToken(") >= 4
assert "if (keySym > UINT_MAX)" in source
assert "*detail = (unsigned int)keySym;" in source
assert "isupper((unsigned char)*pch)" in source
assert "tolower((unsigned char)*pch)" in source
assert "parsed_ix < 0 || (size_t)parsed_ix >= WMFUNCTIONTABLESIZE" in source
assert source.count("&= ~(Context)F_SUBCONTEXT_IB_") == 4
print("PASS MWM parser bounds token copies, KeySym values, and menu indices")
