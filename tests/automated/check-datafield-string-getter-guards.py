#!/usr/bin/env python3
"""Check DataField string exports validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text()
for function in ("df_MakeCopy", "df_WcsMakeCopy"):
    start = source.rindex(function)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "w == NULL || value == NULL" in body, function
    assert "*value = (XtArgVal) NULL" in body, function

print("DataField string getters validate widget and output pointers")
