"""Actual registry list/selection contract, identities, callbacks and lifecycle."""
import ast
import json
import os
from pathlib import Path
import select
import subprocess
import sys
import time
import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib
DBusGMainLoop(set_as_default=True)
fixture, output = map(Path, sys.argv[1:3])
records = []
p = None
passed = False
try:
    address = str(dbus.SessionBus().get_object('org.a11y.Bus', '/org/a11y/bus').GetAddress(dbus_interface='org.a11y.Bus'))
    assert address.startswith('unix:path=' + str(Path(os.environ['XDG_RUNTIME_DIR']).resolve()) + '/')
    bus = dbus.bus.BusConnection(address)
    env = dict(os.environ)
    if len(sys.argv) > 3: env['LD_PRELOAD'] = str(Path(sys.argv[3]).resolve(strict=True))
    p = subprocess.Popen([str(fixture.resolve())], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, env=env)
    context = GLib.MainContext.default()
    def drain(seconds=.04):
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            while context.pending(): context.iteration(False)
            time.sleep(.001)
    def response():
        end = time.monotonic() + 10
        while time.monotonic() < end:
            drain(.001)
            if select.select([p.stdout], [], [], .001)[0]:
                line = p.stdout.readline()
                assert line, 'fixture exited'
                return json.loads(line)
        raise AssertionError('fixture timeout')
    ids = response(); records.append(dict(identity=ids))
    access = 'org.a11y.atspi.Accessible'; selection = 'org.a11y.atspi.Selection'
    action = 'org.a11y.atspi.Action'; props = 'org.freedesktop.DBus.Properties'
    def object_(ref):
        if isinstance(ref, int): ref = (ids['bus'], '/org/a11y/atspi/accessible/' + str(ref))
        return bus.get_object(*ref, introspect=False)
    owner = object_(ids['list']); multi = object_(ids['multiple']); custom = object_(ids['custom'])
    events = []
    bus.add_signal_receiver(lambda *args, **kw: events.append(dict(member=kw['member'], path=kw['path'], args=[str(x) for x in args])),
                            dbus_interface='org.a11y.atspi.Event.Object', member_keyword='member', path_keyword='path', bus_name=ids['bus'])
    def rpc(obj, method, *args, interface=selection):
        return getattr(obj, method)(*args, dbus_interface=interface)
    def children(obj): return [tuple(map(str, x)) for x in rpc(obj, 'GetChildren', interface=access)]
    def name(ref): return str(rpc(object_(ref), 'Get', access, 'Name', interface=props))
    def count(obj): return int(rpc(obj, 'Get', selection, 'NSelectedChildren', interface=props))
    def command(text):
        p.stdin.write(text + '\n'); p.stdin.flush(); result = response(); drain(); records.append(dict(command=text, state=result)); return result
    def stale(ref):
        try: rpc(object_(ref), 'DoAction', 0, interface=action)
        except dbus.DBusException as error:
            assert error.get_dbus_name() == 'org.freedesktop.DBus.Error.UnknownObject', str(error)
        else: raise AssertionError('stale row action targeted a replacement')
    first = children(owner)
    assert int(rpc(owner, 'GetRole', interface=access)) == 31
    assert [name(x) for x in first] == ['Alpha', 'Same', 'Same', '中']
    assert len(set(first)) == 4
    assert all(int(rpc(object_(x), 'GetRole', interface=access)) == 32 for x in first)
    assert selection in list(map(str, rpc(owner, 'GetInterfaces', interface=access)))
    assert rpc(owner, 'SelectChild', 2) and command('state')['selected'] == [3]
    assert count(owner) == 1 and tuple(map(str, rpc(owner, 'GetSelectedChild', 0))) == first[2]
    assert rpc(object_(first[2]), 'GrabFocus', interface='org.a11y.atspi.Component')
    assert rpc(multi, 'ClearSelection') and count(multi) == 0
    # A semantic selection must establish the same native range anchors/cursor
    # as ordinary selection before a following real Shift-navigation gesture.
    baseline = command('state')
    assert rpc(multi, 'SelectChild', 1)
    selected_state = command('state')
    assert selected_state['callbacks'] == baseline['callbacks'] + 1
    subprocess.run(['xdotool', 'windowfocus', '--sync', str(ids['window'])], check=True)
    multi_rows = children(multi)
    assert rpc(object_(multi_rows[1]), 'GrabFocus', interface='org.a11y.atspi.Component')
    focused = list(map(int, rpc(object_(multi_rows[1]), 'GetState', interface=access)))
    assert focused[0] & (1 << 12), focused
    subprocess.run(['xdotool', 'key', '--clearmodifiers', 'shift+Down'], check=True)
    shifted_state = command('state')
    records.append(dict(shift_after_semantic_selection=shifted_state, selection_callback=selected_state))
    assert shifted_state['multiple'] == [2, 3], shifted_state
    assert (selected_state['callback_start'], selected_state['callback_end'], selected_state['callback_cursor']) == (2, 2, 2), selected_state
    subprocess.run(['xdotool', 'key', '--clearmodifiers', 'shift+Up'], check=True)
    assert command('state')['multiple'] == [2]
    assert rpc(multi, 'ClearSelection') and count(multi) == 0
    command('insert'); shifted = children(owner)
    assert shifted[1:] == first and name(shifted[0]) == 'First'
    assert rpc(object_(first[2]), 'DoAction', 0, interface=action)
    assert command('state')['selected'] == [4]
    command('delete'); stale(first[2]); assert first[2] not in children(owner)
    command('replace'); stale(first[0]); assert name(children(owner)[1]) == 'Alpha'
    previous = children(owner); command('reorder'); reordered = children(owner)
    assert [name(x) for x in reordered] == ['Same', 'Alpha', 'Same', '中']
    assert not set(previous).intersection(reordered)
    for ref in previous: stale(ref)
    assert not rpc(owner, 'SelectAll')
    assert rpc(multi, 'SelectAll') and count(multi) == 4
    assert rpc(multi, 'DeselectSelectedChild', 1) and count(multi) == 3
    assert not rpc(multi, 'IsChildSelected', 1)
    assert rpc(multi, 'SelectChild', 1) and count(multi) == 4
    assert rpc(multi, 'DeselectChild', 0) and count(multi) == 3
    assert rpc(multi, 'ClearSelection') and count(multi) == 0
    command('native-select'); assert count(owner) == 1 and rpc(owner, 'IsChildSelected', 1)
    command('disable'); baseline = command('state'); drain(); start = len(events)
    assert not rpc(owner, 'SelectChild', 0)
    assert not rpc(object_(reordered[0]), 'DoAction', 0, interface=action)
    assert command('state') == baseline
    assert not any(x['member'] == 'SelectionChanged' for x in events[start:])
    command('enable'); assert rpc(owner, 'ClearSelection') and count(owner) == 0
    drain(); start = len(events)
    assert not rpc(custom, 'SelectChild', 0)
    assert command('state')['rejected'] == 1
    assert count(custom) == 0
    assert not any(x['member'] == 'SelectionChanged' for x in events[start:])
    command('remove-provider-on-count')
    try: count(custom)
    except dbus.DBusException as error:
        assert error.get_dbus_name() in ('org.freedesktop.DBus.Error.NotSupported', 'org.freedesktop.DBus.Error.UnknownObject'), str(error)
    else: raise AssertionError('reentrantly removed provider returned a count')
    if len(sys.argv) > 3:
        assert rpc(owner, 'SelectChild', 1)
        saved = children(owner)
        result = command('fault-insert')
        assert result['fault_hits'] >= 1 and result['selected'] == [3], result
        # Projection failed closed. Old item must not target the shifted index.
        stale(saved[1])
        command('fault-clear')
        recovered = children(owner)
        assert recovered[1:] == saved and name(recovered[0]) == 'Fault'
        assert rpc(object_(saved[1]), 'DoAction', 0, interface=action)
        assert command('state')['selected'] == [3]
        reordered = recovered
    command('destroy')
    for ref in reordered: stale(ref)
    assert rpc(multi, 'SelectChild', 2) and count(multi) == 1
    drain(); assert any(x['member'] == 'SelectionChanged' for x in events)
    records.append(dict(events=events))
    p.stdin.write('quit\n'); p.stdin.flush(); stdout, stderr = p.communicate(timeout=10)
    assert p.returncode == 0, stderr
    passed = True
finally:
    if p and p.poll() is None: p.terminate(); p.wait(timeout=5)
    output.write_text(json.dumps(dict(passed=passed, records=records, scope='native list/selection protocol and lifecycle only; not actual app/Orca acceptance'), indent=2) + '\n')
