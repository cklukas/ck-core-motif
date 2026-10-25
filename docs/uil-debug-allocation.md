---
nav_exclude: true
---
# UIL debug listing allocation failure

The debug listing formatter first measures a variadic message and then allocates
storage for its complete result.  If that allocation fails, `lst_debug_output`
now abandons that message and returns through the normal `va_end` path.  The
compiler therefore keeps its existing listing state and can report the source
of the failure instead of passing a null buffer to `vsnprintf`.

The source regression in `tests/automated/check-uil-debug-allocation.py` keeps
the null-allocation guard next to the formatter call.  The existing long-output
listing regression continues to exercise the normal formatting path.
