#!/usr/bin/env python3
"""Check Notebook redisplay tab counts guard malformed children."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text(encoding="utf-8")
start = source.index("Redisplay (\n")
end = source.index("/*-", start + 8)
region = source[start:end]
assert region.count("if (child == NULL || child->core.constraints == NULL)") >= 2
assert region.count("nc = NotebookConstraint(child);") >= 2
print("PASS Notebook redisplay tab counts guard malformed child constraints")
