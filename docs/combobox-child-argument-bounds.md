# ComboBox child-argument bounds

During ComboBox initialization, Motif filters `XmNheight` out of the caller's
argument list before creating the private list and text children. The filter
now checks both host `size_t` and Xt `Cardinal` byte limits before multiplying
by `sizeof(Arg)`, and it uses a full-width `Cardinal` index while copying
arguments. An unrepresentable
or failed temporary allocation follows the existing compatibility fallback and
passes the original arguments to child creation instead of writing through a
wrapped allocation.

This preserves normal resource propagation and the public ComboBox ABI while
making oversized or malformed argument lists fail safely.

The source-level regression is `tests/automated/check-combobox-child-args.py`.
