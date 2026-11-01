#!/usr/bin/env python3
"""Check MWM continued configuration lines keep fgets lengths representable."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("unsigned char *\nGetNextLine")
end = source.index("} /* END OF FUNCTION GetNextLine */", start)
region = source[start:end]

assert "size_t\t\t\tremaining;" in region
assert region.count("remaining = (size_t)MAXLINE - (size_t)(string - line);") == 2
assert region.count("remaining <= 1 || remaining > (size_t)INT_MAX") == 2
assert "fgets((char *)string, (int)remaining, cfileP)" in region
print("PASS MWM continued configuration lines bound fgets capacity explicitly")
