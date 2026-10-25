# Scroll-visible callback guards

`IconScrollVisibleCallback` now ignores missing callback data, active workspace
or icon-box state, and missing traversal destinations before calling
`XmScrollVisible`.  Valid traversal callbacks remain unchanged.

`tests/automated/check-scroll-visible-callback-guards.py` protects the contract.
