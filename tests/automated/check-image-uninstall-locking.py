#!/usr/bin/env python3
"""Check that image uninstall does not reacquire its caller-held lock."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
callback_anchor = source.index("/* A hash table map procedure to uninstall an image. */")
callback_start = source.index("UninstallImageMapProc (XmHashKey", callback_anchor)
callback_end = source.index("\n}\n", source.index("return False;", callback_start)) + 3
callback = source[callback_start:callback_end]

assert "_XmRemoveHashEntry (image_set, entry->image_name);" in callback
assert "_XmProcessLock();" not in callback
assert "_XmProcessUnlock();" not in callback

function_start = source.rindex("XmUninstallImage(\n")
function_end = source.index("\n}\n", source.index("return ret_val;", function_start)) + 3
function = source[function_start:function_end]
assert function.count("_XmProcessLock();") == 1
assert "_XmMapHashTable(image_set, UninstallImageMapProc, image);" in function

print("image uninstall map callback relies on the caller-held process lock")
