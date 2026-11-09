# MWM IconBox event-type validation

The MWM IconBox focus and key handlers now verify the event type before reading
the corresponding XEvent union member. Focus feedback accepts only FocusIn and
FocusOut, while the key dispatcher accepts only KeyPress.

This keeps an unexpected event delivery from being interpreted as unrelated
focus or key-event fields. Normal IconBox focus feedback and keyboard commands
are unchanged.

Focused regression check:

    python3 tests/automated/check-keypress-callback-guards.py
