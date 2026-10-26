# SpinBox numeric formatter allocation handling

SpinBox numeric display formatting now initializes its output pointer, checks
the computed size against both host and Xt limits, and returns cleanly when
allocation fails.  Callers can therefore leave the child value unchanged (or
report no generated value) instead of dereferencing a null buffer.

The focused source-contract check is:

    python3 tests/automated/check-spinb-numtostring-allocation.py
