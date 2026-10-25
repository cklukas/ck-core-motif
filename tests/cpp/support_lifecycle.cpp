/* Installed-prefix consumer proof for the WP-08 C++ support prototype. */
#include "motif_support.hpp"

#include <Xm/Label.h>
#include <Xm/PushB.h>

#include <cassert>
#include <exception>
#include <iostream>
#include <stdexcept>

int main(int argc, char **argv)
{
    XtAppContext app = nullptr;
    Widget shell = XtVaAppInitialize(&app, const_cast<char *>("CppSupport"),
                                     nullptr, 0, &argc, argv, nullptr,
                                     XmNwidth, 320, XmNheight, 160, nullptr);
    assert(shell != nullptr);

    Widget label = XtVaCreateManagedWidget("label", xmLabelWidgetClass, shell,
                                          nullptr);
    Widget button = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass,
                                            shell, nullptr);
    XtRealizeWidget(shell);

    motif::string original = motif::string::localized("C++ label");
    motif::string copied(original);
    motif::string moved(std::move(copied));
    assert(original && moved && !copied);

    motif::args values;
    values.add(XmNlabelString, moved.get());
    assert(values.size() == 1);
    values.apply([&](Arg *native, Cardinal count) {
        assert(native != nullptr && count == 1);
        XtSetValues(label, native, count);
    });

    bool sealed = false;
    try {
        values.add(XmNwidth, static_cast<Dimension>(120));
    } catch (const std::logic_error &) {
        sealed = true;
    }
    assert(sealed);

    motif::widget_ref view(label);
    assert(view && view.get() == label);

    unsigned callback_calls = 0;
    std::exception_ptr captured;
    {
        motif::callback_connection connection = motif::callback_connection::connect(
            motif::widget_ref(button), XmNactivateCallback,
            [&](motif::widget_ref callback_widget, XtPointer) {
                assert(callback_widget.get() == button);
                ++callback_calls;
                throw std::runtime_error("callback failure");
            },
            [&](std::exception_ptr error) { captured = error; });
        assert(connection.connected());
        motif::callback_connection moved_connection(std::move(connection));
        assert(!connection.connected() && moved_connection.connected());
        XtCallCallbacks(button, XmNactivateCallback, nullptr);
        XtCallCallbacks(button, XmNactivateCallback, nullptr);
        assert(callback_calls == 2 && captured != nullptr);

        XtDestroyWidget(button);
        assert(!moved_connection.connected());
        moved_connection.reset();
    }

    Widget second = XtVaCreateManagedWidget("second", xmPushButtonWidgetClass,
                                           shell, nullptr);
    motif::callback_connection second_connection =
        motif::callback_connection::connect(
            motif::widget_ref(second), XmNactivateCallback,
            [](motif::widget_ref, XtPointer) {});
    assert(second_connection.connected());
    second_connection.reset();
    assert(!second_connection.connected());
    XtDestroyWidget(second);

    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    std::cout << "PASS C++ support prototype: XmString, args, widget_ref, "
                 "callback exception barrier and destruction ordering\n";
    return 0;
}
