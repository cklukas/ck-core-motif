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

start = source.index("XmFontListEntryGetFont(")
end = source.index("\n}\n", start)
get_font = source[start:end]
assert "_XmAppLock" not in get_font
assert "_XmAppUnlock" not in get_font
assert "_XmProcessLock" not in get_font
assert "_XmProcessUnlock" not in get_font
assert "XmRenditionRetrieve" in get_font

start = source.index("XmFontListEntryGetTag(")
end = source.index("\n}\n", start)
get_tag = source[start:end]
assert "_XmAppLock" not in get_tag
assert "_XmAppUnlock" not in get_tag
assert "_XmProcessLock" not in get_tag
assert "_XmProcessUnlock" not in get_tag
assert "XmRenditionRetrieve" in get_tag

start = source.index("XmFontListEntryCreate_r(")
end = source.index("\n}\n", start)
create_r = source[start:end]
assert "_XmWidgetToAppContext" not in create_r
assert "_XmAppLock" not in create_r
assert "_XmAppUnlock" not in create_r
assert "_XmStringCacheTag" in create_r
assert "XmRenditionCreate" in create_r

start = source.index("XmFontListEntryCreate(")
end = source.index("\n}\n", start)
create = source[start:end]
assert "_XmProcessLock" not in create
assert "_XmProcessUnlock" not in create
assert "_XmStringCacheTag" in create
assert "XmRenditionCreate" in create

print("XmFontList constructors avoid recursive process locking")
