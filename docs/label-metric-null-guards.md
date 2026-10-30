# Safe label metrics for ProgressBar and ToggleSwitch

`XmProgressBar` and `XmToggleSwitch` accept a `NULL` label when an application
clears the label resource.  Their redisplay and geometry paths now measure the
label only when both a render font and an `XmString` are available.  A cleared
label therefore renders as an unlabeled control and reports geometry based on
the control itself, without passing a null string to `XmStringExtent`.

This preserves the existing resource behavior while making label clearing safe
for applications that update resources dynamically.
