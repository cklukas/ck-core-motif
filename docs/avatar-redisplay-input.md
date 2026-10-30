---
nav_exclude: true
---

# Avatar rendering input

Avatar `Redisplay` now validates the widget class before retrieving display and
window state or drawing. Invalid expose dispatch returns safely; normal Avatar
rendering remains unchanged.

Expose processing also requires the widget's graphics context. If X resource
allocation leaves the context unavailable, the callback returns before issuing
`XDrawArc` or `XCopyArea`, avoiding an X error while the widget remains safe to
recreate or destroy.

The focused source contract is checked by
`tests/automated/check-avatar-redisplay-input.py`.
