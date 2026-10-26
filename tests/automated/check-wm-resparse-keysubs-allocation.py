#!/usr/bin/env python3
"""Check WSM key-substitution parsing fails closed on allocation errors."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static void InitKeySubs")
end = source.index("#endif /* WSM */", start)
region = source[start:end]
assert "KeySub *resized;" in region
assert "if (resized == NULL)" in region
assert "if (pKSret == NULL)" in region
assert "if (pKS->pchFrom == NULL)" in region
assert "if (pKS->pchTo == NULL)" in region
assert "*pNumKeySubs = numKS" in region
assert "pKS->pchFrom = NULL" in region
print("PASS WSM key-substitution allocations fail closed")
