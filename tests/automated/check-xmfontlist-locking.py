#!/usr/bin/env python3
"""Check font-list constructors avoid recursive process locking."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmFontList.c").read_text()

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

start = source.index("XmFontListEntryLoad(")
end = source.index("\n}\n", start)
load = source[start:end]
assert "_XmAppLock" not in load
assert "_XmAppUnlock" not in load
assert "_XmStringCacheTag" in load
assert "_XmRenditionCreate" in load

start = source.index("XmFontListAppendEntry(")
end = source.index("\n}\n", start)
append = source[start:end]
assert "_XmAppLock" not in append
assert "_XmAppUnlock" not in append
assert "_XmProcessLock" not in append
assert "_XmProcessUnlock" not in append
assert "XmRenderTableAddRenditions" in append

print("XmFontList constructors avoid recursive process locking")
