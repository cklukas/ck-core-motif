# Indicator drawing size guard

On wide-prototype builds, `XmeDrawIndicator` receives signed integer width and
height parameters but forwards them to internal `Dimension` parameters. A
negative value could therefore become a very large unsigned dimension before
the checkmark or cross renderer saw it.

The public helper now rejects non-positive dimensions before dispatch. Valid
indicator geometry and the Motif API remain unchanged. The regression
`tests/automated/check-drtog-indicator-size-guard.py` covers the boundary.
