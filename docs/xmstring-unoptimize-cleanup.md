# XmString unoptimization cleanup

`Unoptimize` now releases its newly allocated array-entry object when segment
size validation fails before segment storage can be allocated.  This closes an
early-return leak without changing successful conversion behavior.
