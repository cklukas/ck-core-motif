---
nav_exclude: true
---
# Menu recreation allocation

Dynamic menu updates now check the temporary `MenuSpec` pointer array before
initializing it.  If the allocation fails, Mwm reports the condition and
leaves the existing menu widgets untouched.
The element-count multiplication is also bounded before allocation, so an
unrepresentable match-list size is rejected without wrapping the byte count or
exceeding Xt's `Cardinal` allocation width.

`tests/automated/check-menu-recreate-allocation.py` protects this path.
