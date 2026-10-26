#!/usr/bin/env python3
"""Check that charset-table population does not narrow its entry count."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text(encoding="utf-8")
start = text.index("CvtStringToCharSetTable(", 60000)
end = text.index("\n/*ARGSUSED*/", start)
region = text[start:end]

required = (
    "unsigned int numCharsets = 0;",
    "unsigned int i;",
    "i < numCharsets",
)
missing = [item for item in required if item not in region]
if missing:
    raise SystemExit("charset-table index width guard missing: " + ", ".join(missing))
if "(int)numCharsets" in region:
    raise SystemExit("charset-table population still narrows numCharsets to int")

print("charset-table population keeps unsigned entry-count width")
