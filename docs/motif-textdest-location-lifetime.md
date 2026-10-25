# Text drop location lifetime

Text drops can continue through an asynchronous transfer callback. The drop
location is now stored in heap memory owned by the transfer instead of pointing
at a stack-local `XPoint`. The transfer done callback releases it for both
primary and Motif-drop transfers, and allocation failure rejects the transfer
cleanly.

The regression check is
`tests/automated/check-textdest-location-lifetime.py`.
