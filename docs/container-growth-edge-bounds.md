---
nav_exclude: true
---
# Container closest-placement growth edges

When a Container uses closest inclusion, a requested child position beyond the
current content edge contributes enough cell space for the child to fit.  The
content edge is formed from unsigned Motif dimensions and margins, so subtracting
those values before widening can wrap when a transient allocation is smaller than
its margins.  The remainder test also must apply to the widened excess, rather
than to a mixed-precedence unsigned expression.

`RequestSpatialGrowth` now widens both content edges and excess distances before
division.  This preserves the existing cell-growth policy while preventing
underflow and incorrect extra-cell calculations during resize and closest
placement.  `tests/automated/check-container-growth-bounds.py` protects the
contract.
