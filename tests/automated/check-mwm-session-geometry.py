#!/usr/bin/env python3
"""Check MWM session geometry remains signed and representable."""

from pathlib import Path


source = Path("clients/mwm/WmXSMP.c").read_text(encoding="utf-8")
start = source.index("static void\ngetClientGeometry")
end = source.index("\n/*\n *  Assumes: pCD->smClientID", start)
body = source[start:end]

assert "int *clientWd, int *clientHt" in body
assert "unsigned int *clientWd" not in body
assert "pCD->clientWidth <= pCD->baseWidth" in body
assert "pCD->clientHeight <= pCD->baseHeight" in body
assert "pCD->widthInc > 0" in body
assert "pCD->heightInc > 0" in body
assert "unsigned int clientWd" not in source
assert source.count("fprintf(fp, intArg, clientWd);") == 2
assert source.count("fprintf(fp, intArg, clientHt);") == 2

print("PASS MWM session geometry clamps invalid sizes before signed persistence")
