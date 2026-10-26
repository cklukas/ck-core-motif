# MWM function-argument bounds

MWM function parsers now retain `strlen()` results in `size_t` until the
allocation bound is checked against Xt's `Cardinal` width. The `F_Exec`
suffix path also handles an empty argument without indexing before the
allocated string. Multibyte byte counts remain `int`, matching `mblen()`.

The source contract is covered by
`tests/automated/check-wm-resparse-function-args.py`.
