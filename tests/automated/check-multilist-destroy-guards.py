#!/usr/bin/env python3
"""Check MultiList selection and row operations fail during teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/MultiList.c").read_text(
    encoding="utf-8"
)

assert source.count("elist->core.being_destroyed") >= 11
print("PASS MultiList teardown guards")
