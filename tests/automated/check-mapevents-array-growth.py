#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/MapEvents.c").read_text()
assert "#include <string.h>" in source
assert "new_event_types = (int *)XtMalloc" in source
assert "new_keysyms = (KeySym *)XtMalloc" in source
assert "new_modifiers = (Modifiers *)XtMalloc" in source
assert source.count("item_count > UINT_MAX / sizeof") >= 3
assert "if (new_event_types == NULL || new_keysyms == NULL" in source
assert source.count("memcpy(new_") >= 3
assert "*eventTypes = new_event_types;" in source
print("MapEvents parallel-array growth guards present")
