---
nav_exclude: true
---
# Container scroll navigator validation

The auto-scroll timer now rejects an inconsistent scroll-frame result that
reports navigators without an array, and skips null navigator entries before
trait lookup. Valid navigator lists continue to scroll normally.

The source contract is checked by
`tests/automated/check-container-scroll-navigator-inputs.py`.
