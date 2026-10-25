# SpinBox textual trait validation

SpinBox now validates the child widget before looking up its access-textual
trait and checks the specific `setValue` or `getValue` callback before use.
Partial traits therefore leave the child unchanged or report the existing
current-value fallback instead of causing a null function-pointer call.

The regression check is
`tests/automated/check-spinb-text-trait.py`.
