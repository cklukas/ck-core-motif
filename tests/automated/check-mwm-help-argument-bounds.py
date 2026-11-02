#!/usr/bin/env python3
"""Check MWM help arguments stay within the established path contract."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)

copy_start = source.index("CopyHelpArgument(unsigned char *destination")
copy_end = source.index("\n\n\n\f", copy_start)
copy = source[copy_start:copy_end]
assert "if (destination == NULL || source == NULL)" in copy
assert "if (length > MAXWMPATH)" in copy
assert "destination[0] = '\\0';" in copy
assert "memcpy(destination, source, length + 1);" in copy

start = source.index("WmDtGetHelpArgs(char *args,")
end = source.index("} /* END OF FUNCTION WmDtGetHelpArgs */", start)
region = source[start:end]
assert "if (args == NULL || volume == NULL || topic == NULL || argsCount == NULL)" in region
assert "volume[0] = '\\0';" in region
assert "topic[0] = '\\0';" in region
assert "*argsCount = 0;" in region
assert region.count("CopyHelpArgument(") == 2
assert "strcpy ((char*)topic" not in region
assert "strcpy ((char*)volume" not in region

print("PASS MWM help arguments validate outputs and bound token copies")
