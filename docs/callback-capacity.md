# Callback-list capacity bounds

Motif's internal callback list stores its entry count in an unsigned short.
`_XmAddCallback` now refuses to grow a list at that representational limit,
and checks the header-plus-entry byte count before either allocation path.
This prevents count wraparound and keeps the callback array within Xt's
allocation-width contract while preserving normal callback behavior.
