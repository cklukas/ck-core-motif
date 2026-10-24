# Color database RGB parsing

The color selector now checks the result of each RGB channel scan.  Lines that
do not contain three numeric channel values are skipped before their unassigned
storage can be used, so malformed database entries cannot produce arbitrary
colors.
