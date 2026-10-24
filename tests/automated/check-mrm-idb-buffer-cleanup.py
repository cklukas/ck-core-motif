#!/usr/bin/env python3
"""Check failed in-memory IDB opens release their descriptor."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c").read_text(encoding="utf-8")
start = source.index("UrmIdbOpenBuffer (")
end = source.index("\n}\n", start)
region = source[start:end]
header = region.index("result = Idb__HDR_GetHeader")
failure = region.index("if ( result != MrmSUCCESS )", header)
cleanup = region.index("XtFree ((char *)filedesc)", failure)
return_statement = region.index("return result", failure)
assert cleanup < return_statement
print("Mrm in-memory IDB open releases failed descriptors")
