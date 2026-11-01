#!/usr/bin/env python3
"""Check exact source ranges, component selection and malformed annotation failures."""
import importlib.util
import json
from pathlib import Path
import sys
import textwrap
import unittest

ROOT = Path(__file__).resolve().parents[2]
GALLERY = ROOT / "demos/programs/ckmotif-gallery"
sys.path.insert(0, str(GALLERY))
from gallery_source import GallerySource

spec = importlib.util.spec_from_file_location(
    "catalogue", ROOT / "tests/automated/generate-gallery-catalogue.py")
catalogue = importlib.util.module_from_spec(spec)
spec.loader.exec_module(catalogue)


class ExcerptsTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.source = (GALLERY / "gallery_examples.c").read_text()
        cls.parsed = GallerySource(cls.source)
        cls.entries = json.loads((ROOT / "docs/modern-gallery-manifest.json").read_text())["entries"]

    def entry(self, symbol):
        return next(e for e in self.entries if e["class_symbol"] == symbol)

    def test_all_entries_have_exact_ranges_shared_with_docs(self):
        source_lines = self.source.splitlines()
        for entry in self.entries:
            with self.subTest(component=entry["id"]):
                parts = self.parsed.excerpts(entry)
                self.assertEqual(bool(parts), bool(entry["constructible"]))
                docs = "\n".join(catalogue.source_excerpt(ROOT, entry))
                text = self.parsed.text(entry)
                for part in parts:
                    self.assertEqual(part.text, textwrap.dedent(
                        "\n".join(source_lines[part.first - 1:part.last])))
                    self.assertIn(f"#L{part.first}-L{part.last}", docs)
                    self.assertIn(part.text, text)
                self.assertNotIn("gallery_attach_public_callback", text)
                self.assertNotIn("GALLERY-SOURCE-", text)

    def test_button_and_field_context_is_included(self):
        button = self.parsed.text(self.entry("xmPushButtonWidgetClass"))
        for required in ("Apply changes", "disabledState", "defaultState"):
            self.assertIn(required, button)
        self.assertNotIn("XmCreateScrolledList", button)
        self.assertNotIn("xmNotebookWidgetClass", button)
        field = self.parsed.text(self.entry("xmTextFieldWidgetClass"))
        self.assertIn("fieldLabel", field)
        self.assertIn("fieldHelp", field)

    def test_special_factories_and_excluded_classes(self):
        drag = self.parsed.text(self.entry("xmDragOverShellWidgetClass"))
        self.assertIn("gallery_create_drag_source_example", drag)
        self.assertNotIn("XtCreatePopupShell", drag)
        segmented = self.parsed.text(self.entry("xmSegmentedControlWidgetClass"))
        self.assertIn("gallery_create_segmented_control_example", segmented)
        for entry in self.entries:
            if not entry["constructible"]:
                self.assertNotIn("XtVaCreateManagedWidget", self.parsed.text(entry))

    def test_aliased_classes_select_their_actual_factories(self):
        combo = self.parsed.text(self.entry("xmCombinationBox2WidgetClass"))
        self.assertIn("XmListAddItems(XmDropDownGetList(example)", combo)
        self.assertEqual(self.parsed.excerpts(self.entry("xmExt18ListWidgetClass")),
                         self.parsed.excerpts(self.entry("xmMultiListWidgetClass")))

    def test_annotations_only_select_known_inventory_symbols(self):
        known = {e["class_symbol"] for e in self.entries} | {"*"}
        for key in self.parsed.parts:
            self.assertIn(key.split()[1], known)

    def test_malformed_annotations_fail(self):
        begin = "/* GALLERY-SOURCE-BEGIN: construction xmTestWidgetClass */\n"
        end = "/* GALLERY-SOURCE-END */\n"
        valid = begin + "code();\n" + end
        for invalid in (begin, end, begin + end, begin + valid + end,
                        valid + valid, "/* GALLERY-SOURCE-BEGIN */\n",
                        "/* GALLERY-EXAMPLE-BEGIN: first */\ncode();\n"
                        "/* GALLERY-EXAMPLE-END: other */\n"):
            with self.subTest(source=invalid), self.assertRaises(ValueError):
                GallerySource(invalid)


if __name__ == "__main__":
    unittest.main()
