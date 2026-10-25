# Active workspace icon placement

Non-WSM icon-hint processing can run while the active workspace is being
replaced.  Placement now checks `pSD->pActiveWS` before accessing its grid.
When the workspace is unavailable, the requested icon coordinates are kept
and the icon remains unassigned (`NO_ICON_PLACE`) until normal management can
place it safely.
