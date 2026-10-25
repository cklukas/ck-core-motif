# Container outline-layout allocation

Container detail layout builds temporary outline segments and depth points.
Those allocations are now checked before the layout loop dereferences them.
When geometry storage cannot be allocated, the children are still positioned
and the outline is omitted for that pass; the persistent segment count is
cleared so later painting cannot read a missing array.

`tests/automated/check-container-layout-allocation.py` protects the checked
geometry allocations and the outline fallback.
