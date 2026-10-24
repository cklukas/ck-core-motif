#!/usr/bin/env python3
"""Check that XmScale value formatting is size-aware."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Scale.c").read_text()
start = source.index("GetValueString(\n        XmScaleWidget sw,\n        int value,\n        String buffer,\n        size_t buffer_size)\n{")
end = source.index("\n\n\n/************************************************************************\n *\n *  ShowValue", start)
region = source[start:end]
assert "size_t buffer_size" in region
assert "snprintf(buffer, buffer_size" in region
assert "(size_t)formatted >= buffer_size" in region
assert "memmove(buffer + diff + dec_point_size" in region
assert 'sprintf (buffer,"%.*d"' not in region
assert 'sprintf (buffer,"%d", value)' not in region
assert "GetValueString(sw, sw->scale.value, tmpstring, sizeof(tmpstring));" in source
print("XmScale value formatting is bounded for both callers")
