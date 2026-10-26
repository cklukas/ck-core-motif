# MWM feedback-window creation failure

The feedback overlay is optional runtime UI. `UpdateFeedbackWindow` now stops
after a failed `XCreateWindow` instead of continuing into configure/map calls
with `None` as the window ID. Normal feedback updates remain unchanged when
the overlay window is available.
