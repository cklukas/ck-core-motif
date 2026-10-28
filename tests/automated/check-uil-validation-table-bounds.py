#!/usr/bin/env python3
"""Check UIL validation tables reject invalid and unrepresentable sizes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()

assert "#include <stdint.h>" in source
assert "uil_max_arg < 0 || uil_max_reason < 0" in source
assert "SIZE_MAX / sizeof(*arg_seen)" in source
assert "SIZE_MAX / sizeof(*reason_seen)" in source
assert "arg_count > UINT_MAX / sizeof(*arg_seen)" in source
assert "reason_count > UINT_MAX / sizeof(*reason_seen)" in source
assert "XtMalloc ((Cardinal)(arg_count * sizeof(*arg_seen)))" in source
assert "XtMalloc ((Cardinal)(reason_count * sizeof(*reason_seen)))" in source
print("UIL validation table sizes are checked before allocation and narrowing")
