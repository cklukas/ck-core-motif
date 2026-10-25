# Window-configuration occupancy sentinel

The icon move occupancy check now skips `NO_ICON_PLACE` before indexing the
placement list.  Invalid converted coordinates therefore take the existing
no-placement path without a negative array access.

`tests/automated/check-winconf-occupancy-sentinel.py` protects the contract.
