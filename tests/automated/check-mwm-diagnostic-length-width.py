#!/usr/bin/env python3
"""Check MWM diagnostic lengths stay wide through Xt allocation guards."""
from pathlib import Path
for name in ("WmResource.c", "WmInitWs.c", "WmFunction.c", "WmResParse.c", "WmError.c"):
    text = (Path(__file__).parents[2] / "clients/mwm" / name).read_text()
    assert "(unsigned int)messageLength < UINT_MAX" not in text
    assert "(size_t)messageLength < UINT_MAX" in text
print("PASS MWM diagnostic allocation guards avoid pre-narrowing")
