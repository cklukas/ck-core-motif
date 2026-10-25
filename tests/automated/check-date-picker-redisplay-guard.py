#!/usr/bin/env python3
"""Check XmDatePicker Redisplay validates realization and its drawing GC."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DatePicker.c").read_text()
start = source.rindex("static void Redisplay(")
end = source.index("static void Events(", start)
function = source[start:end]

assert "!XtIsRealized(widget) || !picker->date_picker.gc" in function
assert function.index("if (!XtIsRealized") < function.index("XDrawString")
notify = source[source.index("static void notify("):source.index("static Widget shell_ancestor(")]
assert "strcmp(name, XmNdatePickerOpenChangedCallback) == 0" in notify
print("PASS DatePicker redisplay guard")
