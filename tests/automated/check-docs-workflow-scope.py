#!/usr/bin/env python3
"""Check the hosted documentation workflow is not triggered by every commit."""
from pathlib import Path

workflow = (Path(__file__).parents[2] / ".github" / "workflows" / "pages.yml").read_text(
    encoding="utf-8"
)
assert "workflow_dispatch:" in workflow
for event in ("push:", "pull_request:"):
    start = workflow.index(event)
    next_event = [name for name in ("push:", "pull_request:", "workflow_dispatch:")
                  if name != event and workflow.find(name, start + len(event)) >= 0]
    end = min((workflow.find(name, start + len(event)) for name in next_event), default=len(workflow))
    block = workflow[start:end]
    assert "paths:" in block, event
    assert "'docs/**'" in block, event
    assert "'scripts/build-docs.py'" in block, event
    assert "'clients/**'" not in block, event
    assert "'lib/**'" not in block, event
print("PASS documentation workflow is path-scoped with manual dispatch")
