#!/usr/bin/env python3
"""Check XmTimePicker redisplay and callback routing guards."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TimePicker.c").read_text()
start = source.rindex("static void Redisplay(")
end = source.index("static void Events(", start)
function = source[start:end]
notify = source[source.index("static void notify("):source.index("static Boolean set_value_internal(")]

assert "!XtIsRealized(widget) || !picker->time_picker.gc" in function
assert "strcmp(name, XmNtimePickerActivateCallback) == 0" in notify
print("PASS TimePicker redisplay and callback guards")
