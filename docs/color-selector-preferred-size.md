---
nav_exclude: true
---
# ColorSelector preferred size

`XmColorSelector` derives its preferred size by combining all internal child
geometries, borders, and margins. The accumulation now uses a wide type and
saturates at the `Dimension` maximum. A large child or many children can no
longer wrap the preferred size into a smaller value that destabilizes geometry
negotiation.

The focused source contract is covered by
`tests/automated/check-color-selector-preferred-size.py`.
