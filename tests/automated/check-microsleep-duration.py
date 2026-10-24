#!/usr/bin/env python3
"""Check safe handling of nonpositive XmeMicroSleep durations."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(encoding="utf-8")
start = text.index("XmeMicroSleep(long usecs)")
end = text.index("/************************************************************************", start)
region = text[start:end]
assert "if (usecs <= 0)" in region
assert "return 0;" in region
assert "timeoutVal.tv_sec = usecs/1000000" in region
print("XmeMicroSleep rejects nonpositive durations safely")
