#!/usr/bin/env python3
"""Check Mrm FetchSetValues validates its Cardinal-to-int boundary."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwvalues.c").read_text(
    encoding="utf-8"
)
start = source.index("UrmFetchSetValues (MrmHierarchy")
region = source[start:]

guard = "if (num_args > (Cardinal)INT_MAX ||"
assert guard in region
assert "return MrmFAILURE;" in region[region.index(guard):region.index("locargs =")]
assert "arg_count = (int)num_args;" in region
assert "UrmPlistInit (arg_count, &ptrlist)" in region
assert region.index(guard) < region.index("UrmPlistInit (arg_count, &ptrlist)")

print("PASS Mrm FetchSetValues validates Cardinal-to-int argument counts")
