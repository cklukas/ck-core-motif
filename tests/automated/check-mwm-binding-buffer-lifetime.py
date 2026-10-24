#!/usr/bin/env python3
"""Check builtin binding expansion allocation and lifetime handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text()
start = source.index("void\nSetupDefaultResources")
end = source.index("#ifdef NO_MESSAGE_CATALOG", start)
region = source[start:end]
assert region.count('if (buffer == NULL)') == 2
assert 'Warning("Insufficient memory for builtin key bindings");' in region
assert 'Warning("Insufficient memory for builtin button bindings");' in region
assert region.count('XtFree(buffer);') == 2
assert region.count('ParseKeyStr (pSD, (unsigned char *)buffer);') == 1
assert region.count('ParseButtonStr (pSD, (unsigned char *)buffer);') == 1
print("mwm builtin binding buffers are checked and released")
