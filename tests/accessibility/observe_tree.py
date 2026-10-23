"""Real AT-SPI cache/ordered-tree lifetime test; private D-Bus/X session required."""
import json
import os
import pathlib
import select
import subprocess
import sys
import time
import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

DBusGMainLoop(set_as_default=True)

fixture, receipt = map(pathlib.Path, sys.argv[1:3])
started = time.time()
process = None
records = []
passed = False
try:
    address = str(dbus.SessionBus().get_object("org.a11y.Bus", "/org/a11y/bus").GetAddress(dbus_interface="org.a11y.Bus"))
    assert f"/run/user/{os.getuid()}/at-spi/bus" not in address, "desktop accessibility bus reused"
    bus = dbus.bus.BusConnection(address)
    process = subprocess.Popen([str(fixture.resolve())], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    def response():
        assert select.select([process.stdout], [], [], 60)[0], "fixture timeout"
        line = process.stdout.readline()
        assert line, "fixture exited"
        return json.loads(line)
    nodes = response()
    base = "/org/a11y/atspi/accessible/"
    def obj(node):
        return bus.get_object(nodes["bus"], base + ("root" if node == 1 else str(node)), introspect=False)
    cache = bus.get_object(nodes["bus"], "/org/a11y/atspi/cache", introspect=False)
    access = "org.a11y.atspi.Accessible"
    def items():
        return cache.GetItems(dbus_interface="org.a11y.atspi.Cache", timeout=60)
    signals = {"added": set(), "removed": set()}
    def added(item):
        signals["added"].add(str(item[0][1]))
    def removed(reference):
        signals["removed"].add(str(reference[1]))
    bus.add_signal_receiver(added, signal_name="AddAccessible", dbus_interface="org.a11y.atspi.Cache", bus_name=nodes["bus"])
    bus.add_signal_receiver(removed, signal_name="RemoveAccessible", dbus_interface="org.a11y.atspi.Cache", bus_name=nodes["bus"])
    def dispatch_until(kind, expected):
        deadline = time.monotonic() + 15
        context = GLib.MainContext.default()
        while not expected.issubset(signals[kind]) and time.monotonic() < deadline:
            while context.pending():
                context.iteration(False)
            time.sleep(.001)
        assert expected.issubset(signals[kind]), (kind, len(signals[kind]), len(expected))
    initial = len(items())
    previous = 0
    for count in (1000, 10000):
        start = time.monotonic()
        process.stdin.write(str(count) + "\n"); process.stdin.flush()
        group = response()
        parent = int(group["parent"])
        assert parent > previous
        created = time.monotonic()
        children = obj(parent).GetChildren(dbus_interface=access, timeout=60)
        assert len(children) == count
        ids = [int(str(child[1]).rsplit("/", 1)[1]) for child in children]
        assert ids == sorted(set(ids)) and min(ids) > parent
        assert int(obj(ids[-1]).GetIndexInParent(dbus_interface=access)) == count - 1
        queried = time.monotonic()
        allitems = items()
        assert len(allitems) == initial + count + 1
        projected = {str(item[0][1]): item for item in allitems}
        for index in (0, count // 2, count - 1):
            item = projected[base + str(ids[index])]
            assert int(item[3]) == index and int(item[4]) == 0
            assert str(item[2][1]) == base + str(parent)
        expected = {base + str(value) for value in [parent, *ids]}
        dispatch_until("added", expected)
        cached = time.monotonic()
        process.stdin.write("clear\n"); process.stdin.flush()
        assert response()["cleared"]
        assert len(items()) == initial
        dispatch_until("removed", expected)
        try:
            obj(ids[-1]).GetRole(dbus_interface=access)
            raise AssertionError("removed node still callable")
        except dbus.DBusException as error:
            assert error.get_dbus_name().endswith("UnknownObject"), error
        removed = time.monotonic()
        records.append(dict(count=count, create_seconds=created-start, children_seconds=queried-created,
                            cache_seconds=cached-queried, remove_and_cache_seconds=removed-cached,
                            cache_items=len(allitems), dynamic_added=len(expected & signals["added"]), dynamic_removed=len(expected & signals["removed"])))
        previous = ids[-1]
    process.stdin.write("quit\n"); process.stdin.flush()
    stdout, stderr = process.communicate(timeout=10)
    assert process.returncode == 0, stderr
    passed = True
finally:
    if process and process.poll() is None:
        process.terminate(); process.wait(timeout=5)
    receipt.write_text(json.dumps(dict(passed=passed, started=started, ended=time.time(), measurements=records,
                                      scope="real private AT-SPI transport; source-linked diagnostic, not installed cohort acceptance"), indent=2) + "\n")
if not passed:
    raise SystemExit(1)
print(json.dumps(records))
