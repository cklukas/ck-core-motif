#!/usr/bin/env python3
"""Check synthetic-resource merging validates counts and allocation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Synthetic.c").read_text()
start = source.index("_XmBuildResources(")
end = source.index("\n}\n", start) + 2
region = source[start:end]

for needle in (
    "if (wc_resources_ptr == NULL || wc_num_resources_ptr == NULL)",
    "if (wc_num_resources < 0 || sc_num_resources < 0",
    "total_resources > SIZE_MAX / sizeof(XmSyntheticResource)",
    "total_resources > (size_t)UINT_MAX / sizeof(XmSyntheticResource)",
    "if (new_resources == NULL)",
):
    assert needle in region, needle
assert region.index("if (new_resources == NULL)") < region.index("memcpy")
print("PASS synthetic-resource allocation guards present")
