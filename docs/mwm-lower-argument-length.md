# MWM lower-argument traversal

`F_Lower` scans optional family-stacking arguments using the same command
grammar as `F_Raise`.  Its traversal now uses `size_t` directly instead of
subtracting two from `strlen(args)` into an unsigned counter, and stops on an
empty token.  Short and very long argument strings therefore cannot produce an
underflowed or truncated loop bound.
