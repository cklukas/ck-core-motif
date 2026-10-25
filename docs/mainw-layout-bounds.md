# MainWindow layout bounds

MainWindow layout now performs width and height subtraction with signed
operands before checking for negative space and applying its existing minimum
fallbacks.  Very small windows no longer wrap unsigned dimensions into large
child or separator sizes.

The focused regression is `tests/automated/check-mainw-layout-bounds.py`.
