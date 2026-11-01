#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/SegmentedControl.c").read_text()
s = "".join(source.split())
body=s[s.index("BooleanXmSegmentedControlSetSelectedIndex"):s.index("voidXmSegmentedControlClearSelection")]
assert "returnselect_index((XmSegmentedControlWidget)widget,index,notify,NULL)!=SelectionRejected;" in body
assert "if(index< -1" in s or "if(index<-1||index>=(int)control->segmented.item_count)" in s
print("PASS segmented-selection-result")
