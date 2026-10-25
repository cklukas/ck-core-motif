#!/usr/bin/env python3
"""Check that protocol lookup skips null slots."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c"
text = source.read_text(encoding="utf-8")
needle = """   (p_mgr->protocols[i] == NULL ||
\t    p_mgr->protocols[i]->protocol.atom != p_atom))
"""
if needle not in text:
    raise SystemExit("protocol lookup null-slot guard is missing")

print("protocol lookup null-slot guard present")
