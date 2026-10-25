#!/usr/bin/env python3
"""Check the reserved-entry lower bound for Mrm color tables."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmicon.c").read_text(encoding="utf-8")
start = source.index("Urm__RealizeColorTable (Screen")
end = source.find("\nCardinal ", start + 1)
body = source[start:] if end < 0 else source[start:end]
guard = body.index("ctable == NULL || ctable->count < URMColorTableUserMin")
if "return MrmNOT_VALID;" not in body[guard:guard + 100] or guard > body.index("ctable->item"):
    raise SystemExit("Mrm color-table realization does not reject undersized tables")
print("PASS Mrm color-table realization rejects undersized tables")
