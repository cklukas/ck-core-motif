# MWM feedback colormap cleanup

The feedback overlay restores colormap focus when it is hidden. During screen
shutdown there may be no active screen, so `HideFeedbackWindow` now performs
that restoration only when `ACTIVE_PSD` exists; unmapping the feedback window
still occurs normally.
