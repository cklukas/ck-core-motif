---
nav_exclude: true
---
# Text-field drop location lifetime

The text-field destination callback exposes drop coordinates through
`XmDestinationCallbackStruct.location_data`. Those coordinates are now stored
in transfer-owned memory and released by the transfer done callback instead of
pointing at a stack-local `XPoint`. This keeps coordinates valid for the
asynchronous drop transfer and fails cleanly if the allocation cannot be made.

The focused regression is
`tests/automated/check-text-field-drop-location-lifetime.py`.
