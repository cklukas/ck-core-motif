# Accessibility child-index bounds

Accessibility stores child indices as `size_t` but exposes signed `int`
positions to callers and transport notifications.  Both index lookup and
removed-node notifications now use a saturating conversion, preventing large
trees from wrapping into negative or unrelated positions.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-accessible-child-index-bounds.py
```
