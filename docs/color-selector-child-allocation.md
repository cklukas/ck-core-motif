---
nav_exclude: true
---
# ColorSelector child allocation

`XmColorSelector` creates slider, mode-toggle, and color-preview children as
separate widgets. Failed creation now skips callback registration and parent
management safely; slider updates ignore missing scales, and layout returns
until its required presentation children exist. Normal color-list and slider
behavior is unchanged.

The contract is protected by
`tests/automated/check-color-selector-child-allocation.py`.
