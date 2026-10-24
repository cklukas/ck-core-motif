# Standard transfer-target bounds

`XmeStandardTargets` combines Motif's built-in selection targets with a
caller-supplied private-target count.  The count is an `int` for ABI
compatibility, so invalid negative values and totals that cannot fit in an Xt
allocation are rejected before any allocation or display work.  Valid callers
receive the same built-in target order and count as before.

The regression runner compiles the helper against one installed prefix and
runs invalid-count and valid-zero-count cases under Xvfb:

```sh
python3 tests/automated/run-standard-targets-regressions.py \
  /absolute/prefix /absolute/results
```
