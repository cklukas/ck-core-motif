---
nav_exclude: true
---
# PushButton unhighlight bounds

When an unarmed `XmPushButton` uses internal default-button emphasis, its
unhighlight path adds the enhancement and default-shadow thickness to form an
inner clearing delta.  Small widgets previously subtracted that delta from
`Dimension` values without checking the result.

The path now leaves the border unchanged when the delta consumes either
axis.  Valid unhighlight clearing retains its existing geometry and callback
behavior.
