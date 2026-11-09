# MWM preprocessor temporary output

When its optional configuration preprocessor is enabled, MWM creates the
output pathname with `mkstemp()` rather than asking `tmpnam()` for an
unreserved name.  The file is created atomically in `TMPDIR` (or the platform
temporary directory when `TMPDIR` is unset), then the existing preprocessor
command writes its output to that owned pathname.

The command shape and its normal cleanup are unchanged: MWM still opens the
preprocessed result by pathname and removes it through
`DeleteTempConfigFileIfAny()` and configuration-stack cleanup.  Failure to
reserve or close the temporary file cleanly disables preprocessing for that
config read instead of leaving an unowned name behind.

Regression: `tests/automated/check-mwm-preprocessor-tempfile.py`.
