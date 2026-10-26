# MWM stretcher-window initialization

Client records contain one window ID for each resize-border stretcher.
Creation only fills entries for frame parts that are valid for a particular
decoration. `GetClientInfo` now initializes every entry to `None`, so later
frame regeneration cannot interpret uncreated entries as arbitrary X window
IDs.
