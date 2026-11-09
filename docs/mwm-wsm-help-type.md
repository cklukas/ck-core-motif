---
nav_exclude: true
---
# MWM WSM help-record type

Every WSM `WmScreenData` instance owns workspace-help state and initializes it
regardless of whether the optional Panelist frontend is selected. The record
definition is therefore available for all WSM builds rather than being hidden
behind `PANELIST`. This removes an accidental compile-time dependency without
changing the record layout or runtime behavior.
