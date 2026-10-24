#!/usr/bin/env python3
"""Check Mrm writer vector setters reject null non-empty inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
for function, check in (
    ("UrmCWRSetArgChar8Vec", "(num_stg > 0 && stg_vec == NULL)"),
    ("UrmCWRSetArgCStringVec", "(num_cstg > 0 && cstg_vec == NULL)"),
):
    start = text.index(function + " (")
    end = text.find("\n\n\f", start)
    region = text[start:] if end < 0 else text[start:end]
    if check not in region:
        raise SystemExit(f"{function} lacks a null vector guard")
    if region.index(check) > region.index("UrmCWR__GuaranteeSpace"):
        raise SystemExit(f"{function} checks the vector after allocation")

print("Mrm writer vector setters validate non-empty inputs")
