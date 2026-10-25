# Fast-subclass lookup sequencing

The non-threaded fast-subclass macros now keep the base-class extension pointer
in a local variable for each call. Older code assigned the process-global
`_Xm_fastPtr` and dereferenced it in the same macro expression; compiler
evaluation-order diagnostics showed that this could be undefined and made the
lookup unnecessarily shared between callers.

The local helpers preserve the existing null checks, bit-field operations and
public macro names. The source regression is
`tests/automated/check-fast-subclass-sequencing.py`.
