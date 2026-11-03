#!/usr/bin/env python3
"""Check WML resolver diagnostics do not format through fixed buffers unsafely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmlresolve.c").read_text(
    encoding="utf-8"
)
assert "sprintf (errmsg," not in source
assert source.count("snprintf (errmsg, sizeof(errmsg),") == 4
print("PASS WML resolver diagnostics use bounded formatting")
