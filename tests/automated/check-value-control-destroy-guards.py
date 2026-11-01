#!/usr/bin/env python3
"""Check value controls avoid released state during teardown."""

from pathlib import Path


root = Path(__file__).parents[2]
expectations = {
    "lib/Xm/TimePicker.c": ("XmTimePickerGetValue", "XmTimePickerGetRange"),
    "lib/Xm/DatePicker.c": ("XmDatePickerGetDate", "XmDatePickerGetRange", "XmDatePickerIsOpen"),
    "lib/Xm/Calendar.c": ("XmCalendarGetDate", "XmCalendarGetRange", "XmCalendarGetVisibleMonth"),
    "lib/Xm/ToggleSwitch.c": ("XmToggleSwitchGetState", "XmToggleSwitchSetState"),
    "lib/Xm/Skeleton.c": ("XmSkeletonStart", "XmSkeletonStop", "XmSkeletonIsAnimating"),
    "lib/Xm/NumberBox.c": ("XmNumberBoxGetValue", "XmNumberBoxSetValue", "XmNumberBoxIsInvalid", "XmNumberBoxGetTextField"),
    "lib/Xm/Spinner.c": ("XmSpinnerGetActive", "XmSpinnerGetPhase"),
    "lib/Xm/ProgressBar.c": ("XmProgressBarGetValue", "XmProgressBarGetIndeterminate", "XmProgressBarSetIndeterminate"),
    "lib/Xm/ProgressRing.c": ("XmProgressRingGetValue", "XmProgressRingGetIndeterminate", "XmProgressRingSetIndeterminate"),
    "lib/Xm/RatingControl.c": ("XmRatingControlGetValue",),
}

for relative, functions in expectations.items():
    text = (root / relative).read_text(encoding="utf-8")
    for function in functions:
        start = text.index(function)
        assert "widget->core.being_destroyed" in text[start:start + 600], (
            relative, function
        )

print("value controls guard teardown state")
