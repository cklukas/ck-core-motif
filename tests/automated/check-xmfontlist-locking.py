#!/usr/bin/env python3
"""Check font-list constructors avoid recursive process locking."""

from pathlib import Path


source = Path("lib/Xm/XmFontList.c").read_text()

start = source.index("XmFontListCreate(")
end = source.index("\n}\n", start)
create = source[start:end]
assert "_XmProcessLock();" not in create
assert "_XmProcessUnlock();" not in create
assert "_XmStringCacheTag" in create
assert "XmRenditionCreate" in create

start = source.index("XmFontListAdd(")
end = source.index("\n}\n", start)
add = source[start:end]
assert "_XmProcessLock();" not in add
assert "_XmProcessUnlock();" not in add
assert "_XmStringCacheTag" in add
assert "XmRenderTableAddRenditions" in add

print("XmFontList constructors avoid recursive process locking")
