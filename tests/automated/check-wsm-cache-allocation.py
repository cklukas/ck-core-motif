#!/usr/bin/env python3
"""Check that WSM cache allocation failures do not get dereferenced."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/disp.c"
text = source.read_text(encoding="utf-8")

assert 'if (disp_info == NULL)\n\treturn(NULL);' in text
assert 'if (new_disp_info == NULL)' in text
assert 'if (new_screen_info == NULL)' in text
clear_start = text.index("_WSMClearConfigScreenInfo(")
clear_end = text.index("\n}\n", clear_start) + 2
clear_region = text[clear_start:clear_end]
assert "if (disp_info == NULL)" in clear_region
print("WSM display and screen cache allocations are checked")
