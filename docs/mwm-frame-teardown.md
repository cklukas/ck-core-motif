# MWM frame teardown

Frame construction can fail before a client frame window exists. The teardown
helper `FreeClientFrame` now handles both a null client pointer and a `None`
frame ID, and clears the ID after destruction. This makes cleanup safe after a
partial construction failure and prevents repeated teardown from issuing a
`BadWindow` request.
