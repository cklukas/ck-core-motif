---
nav_exclude: true
---
# UIL debug listing allocation failure

The debug listing formatter first measures a variadic message and then allocates
storage for its complete result.  If that allocation fails, `lst_debug_output`
now abandons that message and returns through the normal `va_end` path.  The
compiler therefore keeps its existing listing state and can report the source
of the failure instead of passing a null buffer to `vsnprintf`.  The measured
length is also checked before narrowing it to Xt's `Cardinal` allocation size.
The second formatting pass is checked as well; a conversion failure or length
mismatch frees the temporary buffer and suppresses the incomplete listing line.

The source regression in `tests/automated/check-uil-debug-allocation.py` keeps
the null-allocation guard next to the formatter call.  The existing long-output
listing regression continues to exercise the normal formatting path.
