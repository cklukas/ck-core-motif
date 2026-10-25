#!/usr/bin/env python3
"""Check that WSM requests without a registered callback fail safely."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/recv.c"
text = source.read_text(encoding="utf-8")

needle = "if (screen_info == NULL || screen_info->request_callback == NULL)"
assert needle in text
guard = text[text.index(needle):text.index(needle) + 180]
assert "FreeRequest(&request);" in guard
assert "return(False);" in guard
register_start = text.index("void\nWSMRegisterRequestCallback(")
register_end = text.index("\n}\n", register_start) + 2
register_region = text[register_start:register_end]
assert "if (scr_info == NULL)" in register_region
own_start = text.index("WSMDefaultOwnSelection(")
own_end = text.index("\n}\n", own_start) + 2
own_region = text[own_start:own_end]
assert "if (own_selection == None)" in own_region
print("WSM requests without callbacks are rejected safely")
