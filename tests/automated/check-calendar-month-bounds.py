#!/usr/bin/env python3
"""Ensure Calendar month helpers reject invalid array indices."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Calendar.c").read_text(
    encoding="utf-8"
)
days = source[source.index("static int days_in_month") : source.index("Boolean XmCalendarIsValidDate")]
names = source[source.index("static const char *month_name") : source.index("static Position bounded_position")]
assert "if (month < 1 || month > 12)" in days
assert "month >= 1 && month <= 12 ? names[month - 1] : \"\"" in names
print("PASS Calendar month helpers validate bounds")
