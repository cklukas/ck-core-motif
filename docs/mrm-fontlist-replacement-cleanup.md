# Mrm legacy font-list replacement cleanup

Old UID font lists are converted into a newly allocated current-format list.
If serialized input is malformed and the fixup step fails, each caller now
releases that replacement before discarding the value.  Widget `SetValues`,
widget creation, callback literal handling, and resource-context reads retain
their existing successful conversion behavior while failure paths no longer
leak temporary storage or fix up an already converted list twice.

The focused source regression is
`tests/automated/check-mrm-fontlist-allocation.py`.
