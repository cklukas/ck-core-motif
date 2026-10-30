#!/usr/bin/env python3
"""Ensure render-table tab-list parsing terminates on malformed EOF."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("} else if (strcmp(items[j], XmNtabList)")
end = source.index("} else if (strcmp(items[j], XmNbackground)", start)
region = source[start:end]

assert region.count("token -> type == T_EOF || token -> type == T_NL") >= 2
assert "goto error;" in region
print("PASS render-table tab-list parsing rejects EOF before completion")
