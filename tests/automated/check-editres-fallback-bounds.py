#!/usr/bin/env python3
"""Check Editres integer fallback formatting is bounded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)

assert "GetResourceString(Widget w, const char * name, char * buffer, size_t buffer_size)" in source
assert "char buffer[1024];" in source
assert "sizeof(buffer)" in source
assert source.count("snprintf(buffer, buffer_size") >= 3
assert "sprintf(&buffer[strlen(buffer)]" not in source
print("EditRes fallback formatting is bounded")
