#!/usr/bin/env python3
"""Ensure Xmfuncs keeps fortified platform compatibility macros intact."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmfuncs.h").read_text(
    encoding="utf-8"
)
portable = source[source.index("#if (__STDC__"):source.index("#else\n#ifdef sgi")]

assert "#ifndef bcopy\n#define bcopy(b1,b2,len) memmove(b2, b1, (size_t)(len))\n#endif" in portable
assert "#ifndef bzero\n#define bzero(b,len) memset(b, 0, (size_t)(len))\n#endif" in portable

print("PASS Xmfuncs preserves platform bcopy and bzero compatibility macros")
