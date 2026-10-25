# Scale value redisplay guards

`XmScale` renders its optional value label from a derived value region,
foreground GC, font resource, and child scrollbar. During realization or
resource replacement those objects can be temporarily unavailable, while the
expose path may still call `ShowValue`.

`ShowValue` now returns until the widget is realized, the value region and
foreground GC exist, the scrollbar child is present, and the active font
resource is valid. Normal value formatting, positioning, clearing, and
rendering remain unchanged once those prerequisites are available.

The focused source contract is checked by
`tests/automated/check-scale-value-redisplay-guards.py`.
