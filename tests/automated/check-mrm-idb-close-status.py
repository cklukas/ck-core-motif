#!/usr/bin/env python3
"""Check Mrm IDB close paths free state and propagate status."""

from pathlib import Path

root = Path(__file__).parents[2]
low = (root / "lib" / "Mrm" / "MrmIfile.c").read_text(encoding="utf-8")
top = (root / "lib" / "Mrm" / "MrmItop.c").read_text(encoding="utf-8")

start = low.index("Idb__FU_CloseFile (")
end = low.index("\n}\n", start)
region = low[start:end]
close_failure = region.index("if (status != 0)")
assert "XtFree (file_id->name)" in region[close_failure:]
unlink = region.index("status = unlink")
assert "if (status != 0)" in region[unlink:]

start = top.index("\nUrmIdbCloseFile (")
end = top.index("\nUrmIdbGetIndexedResource (", start)
region = top[start:end]
close_call = region.index("result = Idb__FU_CloseFile")
assert "return result" in region[close_call:]
print("Mrm IDB close paths clean up and propagate status")
