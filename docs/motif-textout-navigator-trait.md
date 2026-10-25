# TextOut navigator callback validation

TextOut now validates horizontal, vertical and drag navigator traits before
callback dispatch. Complete navigator traits retain normal scrolling, while
partial traits are ignored safely instead of causing null function-pointer
calls.

The regression check is
`tests/automated/check-textout-navigator-trait.py`.
