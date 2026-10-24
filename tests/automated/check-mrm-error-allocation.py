#!/usr/bin/env python3
"""Check Mrm memory-mode error reporting handles replacement failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmerror.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__UT_Error (")
end = text.index("\n\n\f", start)
region = text[start:end]

if "String\t\tnew_msg" not in region:
    raise SystemExit("error reporting has no temporary replacement pointer")
alloc = region.index("new_msg = (String) XtMalloc")
guard = region.index("if (new_msg != NULL)", alloc)
publish = region.index("urm__latest_error_msg = new_msg", guard)
if guard < alloc or publish < guard:
    raise SystemExit("error message replacement is published before allocation succeeds")
if "strcpy (new_msg, msg)" not in region:
    raise SystemExit("error message is not copied into the checked replacement")

print("Mrm error reporting preserves state on allocation failure")
