#!/usr/bin/env python3
"""Check resolved Mrm paths do not copy into a fixed-size scratch buffer."""

from pathlib import Path


hierarchy = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c").read_text(
    encoding="utf-8"
)
if "char\t\t\tdummy[300]" in hierarchy or "char dummy[300]" in hierarchy:
    raise SystemExit("resolved path still uses a fixed-size scratch buffer")
if "UrmIdbOpenFileRead (resolvedname, os_ext, file_id_return, NULL)" not in hierarchy:
    raise SystemExit("resolved path does not omit the unused output buffer")

low_level = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c").read_text(
    encoding="utf-8"
)
if "if (name == NULL || file_id == NULL)" not in low_level:
    raise SystemExit("low-level file open still rejects a null optional output")
if "if (returned_fname != NULL)" not in low_level:
    raise SystemExit("low-level file open does not guard optional output")

print("Mrm resolved paths avoid fixed-size output copies")
