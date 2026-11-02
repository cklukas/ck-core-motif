# ScrollBar callback-list ownership

ScrollBar obtains the public, null-terminated callback array through
`XtGetValues` and dispatches an owned snapshot with
`_XmCallCallbacksUntilDestroyed`. It never interprets Xt’s private compiled
callback storage as an array. Snapshot allocation is bounded and checked before
copying.

A callback may add or remove callbacks during delivery. The current delivery
uses its original snapshot; the next delivery observes those changes. A destroy
watch stops delivery if a callback destroys the widget. This also covers timer
callbacks, where destruction can happen immediately rather than being deferred
until an X event finishes dispatching. Keeping Xt’s compiled list in flight
across immediate widget destruction can otherwise free that list twice.

Scroll-frame navigation separately watches the owner and every navigator in its
snapshot. It stops after destruction and checks membership before notifying each
navigator, so callbacks can safely destroy the frame or remove another navigator.

`check-scrollbar-callback-snapshot.py` and
`check-callback-dispatch-allocation.py` guard the dispatch contract. Native
`check-modern-scale.c` checks observer closures and callback mutation during
delivery. `check-scroll-lifetime.c` exercises timer teardown, frame destruction,
and auto-drag enable/disable transitions. Both run in the macOS modern audit.
