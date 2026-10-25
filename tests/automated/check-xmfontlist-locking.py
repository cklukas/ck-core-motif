#!/usr/bin/env python3
"""Check font-list constructors avoid recursive process locking."""

from pathlib import Path


source = Path("lib/Xm/XmFontList.c").read_text()

start = source.index("XmFontListInitFontContext(")
end = source.index("\nBoolean \nXmFontListGetNextFont", start)
init_context = source[start:end]
assert "if (p == NULL)" in init_context
assert "return (FALSE);" in init_context

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
assert "typeReturn == NULL" in get_font

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

start = source.index("XmFontListCreate_r(")
end = source.index("\n}\n", start)
create_r_font = source[start:end]
assert "_XmWidgetToAppContext" not in create_r_font
assert "_XmAppLock" not in create_r_font
assert "_XmAppUnlock" not in create_r_font
assert "_XmStringCacheTag" in create_r_font
assert "XmRenditionCreate" in create_r_font

start = source.index("XmeRenderTableGetDefaultFont(")
end = source.index("\n}\n", start)
default_font = source[start:end]
assert "_XmAppLock" not in default_font
assert "_XmAppUnlock" not in default_font
assert "_XmProcessLock" not in default_font
assert "_XmProcessUnlock" not in default_font
assert "_XmFontListSearch" in default_font

start = source.index("Boolean \nXmFontListGetNextFont(")
end = source.index("\nvoid\nXmFontListFreeFontContext", start)
next_font = source[start:end]
next_font_body = next_font[next_font.index("rend = _XmRTRenditions(table)"):]
assert next_font_body.index("FLContextIndex(context)++") < next_font_body.index("_XmProcessUnlock();")
assert next_font_body.index("_XmRenditionCopy(rend, True)") < next_font_body.index("_XmProcessUnlock();")
assert next_font_body.index("_XmProcessUnlock();") < next_font_body.index("_XmGetFirstFont(retained)")
assert next_font_body.index("XmRenditionFree(retained)") > next_font_body.index("XmRenditionRetrieve(retained, args, n)")
assert "if (*charset == NULL)" in next_font_body

start = source.index("XmFontListRemoveEntry(")
end = source.index("\nXmFontListEntry\nXmFontListEntryLoad", start)
remove = source[start:end]
retrieve = remove.index("XmRenditionRetrieve(entry, args, n);")
lock = remove.index("_XmProcessLock();")
assert retrieve < lock
assert remove.index("tags[0] = XtNewString(tags[0]);") < lock
assert "if (tags[0] == NULL)" in remove
assert "XtFree(tags[0]);" in remove

start = source.index("XmFontListEntryCreate(")
end = source.index("\n}\n", start)
create = source[start:end]
assert "_XmProcessLock" not in create
assert "_XmProcessUnlock" not in create
assert "_XmStringCacheTag" in create
assert "XmRenditionCreate" in create

print("XmFontList constructors avoid recursive process locking")
