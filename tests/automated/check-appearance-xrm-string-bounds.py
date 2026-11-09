#!/usr/bin/env python3
"""Check appearance-profile Xrm conversion keeps string sizes representable."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Appearance.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nappearance_string_value")
end = source.index("static Boolean\nprofile_property_value", start)
helper = source[start:end]
profile_start = source.index("static Boolean\nprofile_property_value")
profile_end = source.index("/* Cached gadget resources", profile_start)
profile = source[profile_start:profile_end]

assert "length == SIZE_MAX || length + 1 > UINT_MAX" in helper
assert "value->size = (unsigned int)(length + 1);" in helper
assert profile.count("appearance_string_value(text, &from)") == 3
assert "from.size = (unsigned int)strlen(text) + 1" not in profile
assert "*value_return = (XtArgVal)pixel;" in profile
assert "? True : False" in source[source.index("appearance_database_typed_value"):]

print("PASS appearance Xrm string and protocol conversions are bounded")
