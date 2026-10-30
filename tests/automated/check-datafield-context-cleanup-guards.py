#!/usr/bin/env python3
"""Check DataField context cleanup validates callback-owned context state."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text()
start = source.rindex("df_FreeContextData(")
end = source.index("\n}", start)
body = source[start:end]
assert "ctx_data == NULL" in body
assert "ctx_data->screen == NULL" in body
assert "XtFree((char *) ctx_data)" in body

print("DataField context cleanup validates client data and screen")
