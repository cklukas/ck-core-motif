# MWM active icon-label show preconditions

Showing the optional active icon label requires both the helper label window
and the icon frame to exist. `ShowActiveIconText` now checks the client and
screen pointers plus both window IDs before querying frame attributes or
reparenting/configuring the label. This keeps a failed icon allocation or
teardown from producing a `BadWindow` request.
