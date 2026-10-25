---
nav_exclude: true
---

# Segmented gallery example

The native gallery now supplies a three-item public model to its segmented
control example.  `XmSegmentedControlSetItems` used to manage each toggle as
it was created; the composite layout callback could run before later item
slots had a button and crash while querying a null child.  The toolkit now
manages the complete button set only after construction finishes.

The gallery's `Overview`, `Details`, and `Activity` segments therefore render
as a real selectable control and can be exercised by the native integration
test rather than appearing as an empty metadata card.
