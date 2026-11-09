#!/usr/bin/env python3
"""Check MWM client sorting validates its list before qsort."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmXSMP.c").read_text(
    encoding="utf-8"
)
start = source.index("SortClientListByWorkspace")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "clients != NULL && nClients > 0" in region
assert "qsort((void *)clients, (size_t)nClients" in region
assert "qsort((void *)clients, nClients" not in region

compare_start = source.index("static int\nclientWorkspaceCompare")
compare_end = source.index("\n}\n\n/*", compare_start)
compare = source[compare_start:compare_end]
assert "if (pCD1 == NULL)" in compare
assert "if (pCD1->pSD == NULL)" in compare
assert "pCD1->pWsList == NULL" in compare
print("PASS MWM client sorting validates and widens its count")
