#!/usr/bin/env python3
"""Check that MWM builtin bindings tolerate a missing root-menu resource."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
start = source.index("if (pSD->keyBindings == builtinKeyBindingsName)")
end = source.index("#ifdef NO_MESSAGE_CATALOG", start)
region = source[start:end]
assert region.count('root_menu = pSD->rootMenu ? pSD->rootMenu : "DefaultRootMenu";') == 2
assert "strlen(root_menu)" in region
assert "sprintf(buffer, builtinButtonBindings, root_menu);" in region
print("PASS MWM builtin bindings use a root-menu fallback")
