#!/usr/bin/env python3
"""Ensure localized MWM system-menu copies validate Xt allocation width."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
start = text.index("void InitBuiltinSystemMenu(void)")
end = text.index("SetupDefaultResources (pSD)", start)
function = text[start:end]
assert "size_t dsm_size;" in function
assert "dsm_size = strlen(dsm);" in function
assert "dsm_size >= UINT_MAX" in function
assert "XtMalloc ((Cardinal)(dsm_size + 1))" in function
assert "XtMalloc ((unsigned int) (strlen(dsm) + 1))" not in function
print("MWM localized system-menu allocation is width-checked")
