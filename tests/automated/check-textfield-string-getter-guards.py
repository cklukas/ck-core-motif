#!/usr/bin/env python3
"""Check TextField synthetic string/context callbacks reject invalid inputs."""

from pathlib import Path


source = Path("lib/Xm/TextF.c").read_text(encoding="utf-8")

for signature in ("MakeCopy(Widget w", "WcsMakeCopy(Widget w"):
    start = source.rindex(signature)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "w == NULL || value == NULL" in body
    assert "(*value) = (XtArgVal) NULL" in body

start = source.rindex("FreeContextData(Widget w")
end = source.index("\n}", start)
body = source[start:end]
assert "ctx_data == NULL" in body
assert "ctx_data->screen == NULL" in body
assert "XtFree((char *) ctx_data)" in body

print("TextField string/context callback guards present")
