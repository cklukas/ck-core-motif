#!/usr/bin/env python3
"""Check accessibility values never read failed getter output."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text(
    encoding="utf-8"
)


def block(marker: str, next_marker: str) -> str:
    start = source.rindex(marker)
    return source[start:source.index(next_marker, start)]


calendar = block("if (XmIsCalendar(w))", "if (XmIsDatePicker(w))")
assert "XmCalendarDate date = {0};" in calendar
assert "have_date = XmCalendarGetDate(w, &date);" in calendar
assert "i->has_value = have_date;" in calendar
assert "if (have_date)" in calendar
assert calendar.index("if (have_date)") < calendar.index("i->current_value = date.year;")

date_picker = block("if (XmIsDatePicker(w))", "if (XmIsTimePicker(w))")
assert "XmDatePickerDate date = {0};" in date_picker
assert "have_date = XmDatePickerGetDate(w, &date);" in date_picker
assert "i->has_value = have_date;" in date_picker
assert date_picker.index("if (have_date)") < date_picker.index("i->current_value = date.year;")

time_picker = block("if (XmIsTimePicker(w))", "if (XmIsDataGrid(w))")
assert "XmTimePickerValue value = {0};" in time_picker
assert "have_value = XmTimePickerGetValue(w, &value);" in time_picker
assert "i->has_value = have_value;" in time_picker
assert time_picker.index("if (have_value)") < time_picker.index("i->current_value = value.hour")

breadcrumb = block("if (XmIsBreadcrumbBar(w))", "if (XmIsTreeView(w))")
assert "Cardinal count = XmBreadcrumbBarGetSegmentCount(w);" in breadcrumb
assert "bounded_index_max(count, 0)" in breadcrumb

segmented = block("if (XmIsSegmentedControl(w))", "if (XmIsChip(w))")
assert "Cardinal count = XmSegmentedControlGetItemCount(w);" in segmented
assert "bounded_index_max(count, -1)" in segmented

helper = source[source.index("static int bounded_index_max"):source.index("static int bounded_text_position")]
assert "count > (Cardinal)INT_MAX ? INT_MAX : (int)count - 1" in helper

print("Accessibility values are initialized and indexed ranges are bounded")
