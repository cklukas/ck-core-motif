#!/usr/bin/env python3
"""Check Editres diagnostic message sizing is width-safe."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)
start = source.index('\nFormatMessage(')
end = source.index('\n/*\tFunction Name: HandleToolkitErrors', start)
region = source[start:end]

assert "int needed;" in region
assert "vsnprintf(NULL, 0, format, ap)" in region
assert "malloc((size_t)needed + 1)" in region
assert "vsnprintf(result, (size_t)needed + 1, format, ap)" in region
print("PASS Editres error-message sizing is width-safe")
