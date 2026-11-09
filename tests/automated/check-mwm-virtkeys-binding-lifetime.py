#!/usr/bin/env python3
"""Check MWM virtual-key binding copies use their Xt allocation lifetime."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmInitWs.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean\nVirtKeys4DIN(")
end = source.index("\n}\n#endif /* NO_HP_KEY_REMAP */", start)
body = source[start:end]

assert "bounded_prop = (String)XtMalloc((Cardinal)num_items + 1);" in source
assert "XFree( bindingsString)" not in body
assert body.count("XtFree( bindingsString) ;") == 3

print("PASS MWM virtual-key binding copies have one matching XtFree")
