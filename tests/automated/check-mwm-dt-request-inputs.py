#!/usr/bin/env python3
"""Check that the MWM DT request helper validates its inputs."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProperty.c").read_text(
    encoding="utf-8"
)
start = source.index("GetDtWmRequest (")
end = source.index("\n} /* END OF FUNCTION GetDtWmRequest */", start)
region = source[start:end]

assert "pSD == NULL || pszReq == NULL || pmore == NULL" in region
assert "*pszReq = NULL;" in region
assert "*pmore = False;" in region
assert region.index("pSD == NULL") < region.index("XGetWindowProperty")
print("PASS MWM DT request outputs and screen data are validated")
