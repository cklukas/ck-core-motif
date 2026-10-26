#!/usr/bin/env python3
"""Check that MWM property readers reject null client records safely."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProperty.c").read_text(
    encoding="utf-8"
)
normal_start = source.index("SizeHints * \nGetNormalHints")
normal_end = source.index("\n} /* END OF FUNCTION GetNormalHints */", normal_start)
normal = source[normal_start:normal_end]
mwm_start = source.index("PropMwmHints *\nGetMwmHints")
mwm_end = source.index("\n} /* END OF FUNCTION GetMwmHints */", mwm_start)
mwm = source[mwm_start:mwm_end]

assert "if (pCD == NULL)" in normal
assert "sizeHints.icccVersion = ICCC_UNKNOWN;" in normal
assert "return (&sizeHints);" in normal
assert "if (pCD == NULL)" in mwm
assert "return (NULL);" in mwm
print("PASS MWM property readers fail safely for null client records")
