#!/usr/bin/env python3
"""Check WSM target helpers handle unavailable display metadata."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/recv.c"
text = source.read_text(encoding="utf-8")

assert text.count("if (disp_info == NULL)") >= 2
assert "*len_ret = 0;" in text
assert "return(NULL);" in text
assert "if (len_ret == NULL)" in text
assert "if (list == NULL)" in text
print("WSM target helpers handle unavailable display metadata")
