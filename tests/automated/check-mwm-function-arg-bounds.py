#!/usr/bin/env python3
"""Check that menu function arguments are dynamically sized."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text()
helper_start = source.index("static String\nMakeMenuFunctionArgs")
helper_end = source.index("\nstatic void InsertTreeOnClient", helper_start)
helper = source[helper_start:helper_end]
assert "snprintf(NULL, 0, \"<%s>\", tree->name)" in helper
assert "snprintf(NULL, 0, \"%d %ld %ld\"" in helper
prototype_start = source.index(
    "static void InsertTreeOnClient (WmScreenData *pSD, ClientData *pCD,",
    helper_end + 1,
)
start = source.index(
    "static void InsertTreeOnClient (WmScreenData *pSD, ClientData *pCD,",
    prototype_start + 1,
)
end = source.index("\n}\n\n", start)
region = source[start:end]
assert "char funcarg_buf[256]" not in region
assert region.count("XtFree(funcarg_buf);") == 2
assert "MakeMenuFunctionArgs(tree, pCD, selection)" in region
print("mwm menu function arguments are dynamically bounded")
