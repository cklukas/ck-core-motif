# XmIm registration snapshot cleanup

`XmIm` copies input-method resources such as font/render tables, preedit
callbacks, spot locations, and areas so they survive input-context recreation.
When the per-widget snapshot array cannot grow, it now releases every newly
owned copy before continuing. This covers both an allocation failure and the
legacy `Cardinal` size limit.

The focused source-contract regression check is
`tests/automated/check-xmim-registration-owned-value-cleanup.py`.
