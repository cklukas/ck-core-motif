#!/usr/bin/env python3
"""Check Mrm resource-context construction validates its output pointer."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmcontext.c").read_text(encoding="utf-8")
start = source.index("UrmGetResourceContext (")
end = source.index("\n}\n", start)
region = source[start:end]
guard = region.index("if ( context_id_return == NULL )")
assert region.find("*context_id_return", guard + 1) >= 0
print("Mrm resource-context construction validates its output pointer")
