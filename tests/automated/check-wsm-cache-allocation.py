#!/usr/bin/env python3
"""Check that WSM cache allocation failures do not get dereferenced."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/disp.c"
text = source.read_text(encoding="utf-8")

assert 'if (disp_info == NULL)\n\treturn(NULL);' in text
assert 'if (new_disp_info == NULL)' in text
assert 'if (new_screen_info == NULL)' in text
print("WSM display and screen cache allocations are checked")
