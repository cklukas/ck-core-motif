# MWM matte shadow GC initialization

Matte shadow GCs are supplied later by client-resource processing and are
optional for frames without a matte. `GetClientInfo` now initializes both
handles to `NULL`, so early construction or failure cleanup cannot consume
indeterminate GC values.
