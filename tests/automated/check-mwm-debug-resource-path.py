#!/usr/bin/env python3
"""Check MWM debug resource-dump paths use bounded formatting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmFunction.c").read_text(
    encoding="utf-8"
)
start = source.index('if (!(strcmp(subFcn, "dump_resources")))')
end = source.index("\n    return (True);", start)
region = source[start:end]
assert 'snprintf(szRes, sizeof(szRes)' in region
assert 'renderedLength < 0' in region
assert 'sprintf (szRes' not in region
print("PASS MWM debug resource paths use bounded formatting")
