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
assert "size_t dec_point_size;" in region
assert "dec_point_size = strlen(loc_values->decimal_point);" in region
assert "(size_t)formatted + dec_point_size >= buffer_size" in region
assert "int diff, dec_point_size, formatted;" not in region
assert "memmove(buffer + diff + dec_point_size" in region
show_value_start = source.index("static void \nShowValue(")
show_value_end = source.index("static void \nCalcScrollBarData(", show_value_start)
show_value = source[show_value_start:show_value_end]
assert "int buffer_length = (int)strlen(buffer);" in show_value
assert "XTextExtents (sw->scale.font_struct, buffer, buffer_length," in show_value
assert "x, y, buffer, buffer_length);" in show_value
assert 'sprintf (buffer,"%.*d"' not in region
assert 'sprintf (buffer,"%d", value)' not in region
assert "GetValueString(sw, sw->scale.value, tmpstring, sizeof(tmpstring));" in source
print("XmScale value formatting is bounded for both callers")
