#!/usr/bin/env python3
"""Check that Mrm write-open clears its output descriptor before work."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItopw.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmIdbOpenFileWrite (String")
end = text.index("\n}", start)
body = text[start:end]
clear = body.index("*file_id_return = NULL;")
open_call = body.index("Idb__FU_OpenFile", clear)
if clear > open_call:
    raise SystemExit("write-open clears its output after attempting the open")
print("PASS Mrm write-open clears its output descriptor before work")
