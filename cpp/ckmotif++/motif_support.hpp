/*
 * Optional C++17 support for the native Xt/Motif ABI.
 *
 * This header is deliberately a thin ownership layer.  It does not replace
 * libXm's C object model, retain Widgets, create a second event loop, or make
 * Xt safe to call from worker threads.  All widget and connection operations
 * belong to the owning Xt application thread.
 */
#ifndef CK_MOTIF_SUPPORT_HPP
#define CK_MOTIF_SUPPORT_HPP

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include <cstddef>
#include <exception>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace motif {

/* Move/copy ownership for one XmString value. */
class string {
public:
    string() noexcept = default;

    static string localized(const char *text)
    {
        if (text == nullptr)
            return string();
        return string(XmStringCreateLocalized(const_cast<char *>(text)), adopt);
    }

    string(const string &other)
        : value_(other.value_ == nullptr ? nullptr : XmStringCopy(other.value_))
    {
    }

    string(string &&other) noexcept : value_(other.value_)
    {
        other.value_ = nullptr;
    }

    string &operator=(const string &other)
    {
        if (this != &other) {
            string copy(other);
            swap(copy);
        }
        return *this;
    }

    string &operator=(string &&other) noexcept
    {
        if (this != &other) {
            reset();
            value_ = other.value_;
            other.value_ = nullptr;
        }
        return *this;
    }

    ~string() { reset(); }

    void swap(string &other) noexcept
    {
        XmString value = value_;
        value_ = other.value_;
        other.value_ = value;
    }

    XmString get() const noexcept { return value_; }
    explicit operator bool() const noexcept { return value_ != nullptr; }

private:
    struct adopt_t {};
    static constexpr adopt_t adopt{};

    explicit string(XmString value, adopt_t) noexcept : value_(value) {}

    void reset() noexcept
    {
        if (value_ != nullptr)
            XmStringFree(value_);
        value_ = nullptr;
    }

    XmString value_ = nullptr;
};

/* Ordered, one-shot storage for an Xt Arg array. */
class args {
public:
    args() = default;
    args(const args &) = delete;
    args &operator=(const args &) = delete;
    args(args &&) = default;
    args &operator=(args &&) = default;

    template <typename T>
    void add(const char *name, T value)
    {
        static_assert(std::is_integral<T>::value || std::is_pointer<T>::value,
                      "Xt argument values must be integral or pointer values");
        if (name == nullptr)
            throw std::invalid_argument("Xt resource name must not be null");
        if (sealed_)
            throw std::logic_error("Xt argument list is already in use");
        entries_.push_back(entry{std::string(name), to_arg(value)});
    }

    std::size_t size() const noexcept { return entries_.size(); }

    /* The callback must not retain the Arg pointer after it returns. */
    template <typename Function>
    decltype(auto) apply(Function &&function)
    {
        if (sealed_)
            throw std::logic_error("Xt argument list is already in use");
        if (entries_.size() > std::numeric_limits<Cardinal>::max())
            throw std::length_error("Xt argument list exceeds Cardinal capacity");

        std::vector<Arg> native;
        native.reserve(entries_.size());
        for (const entry &item : entries_)
            native.push_back(Arg{const_cast<char *>(item.name.c_str()), item.value});

        /* Seal only after native construction succeeds.  An allocation
         * exception therefore leaves the object retryable and no callback
         * can observe a partially built array. */
        sealed_ = true;
        return std::forward<Function>(function)(
            native.empty() ? nullptr : native.data(),
            static_cast<Cardinal>(native.size()));
    }

private:
    struct entry {
        std::string name;
        XtArgVal value;
    };

    template <typename T>
    static XtArgVal to_arg(T value) noexcept
    {
        if constexpr (std::is_pointer<T>::value)
            return reinterpret_cast<XtArgVal>(value);
        else
            return static_cast<XtArgVal>(value);
    }

    std::vector<entry> entries_;
    bool sealed_ = false;
};

/* A deliberately non-owning view of a Widget. */
class widget_ref {
public:
    widget_ref() noexcept = default;
    explicit widget_ref(Widget widget) noexcept : widget_(widget) {}

    Widget get() const noexcept { return widget_; }
    explicit operator bool() const noexcept { return widget_ != nullptr; }

private:
    Widget widget_ = nullptr;
};

/* RAII for one Xt callback registration with an exception barrier. */
class callback_connection {
public:
    using callback = std::function<void(widget_ref, XtPointer)>;
    using error_sink = std::function<void(std::exception_ptr)>;

    callback_connection() noexcept = default;
    callback_connection(const callback_connection &) = delete;
    callback_connection &operator=(const callback_connection &) = delete;

    callback_connection(callback_connection &&other) noexcept : state_(other.state_)
    {
        other.state_ = nullptr;
    }

    callback_connection &operator=(callback_connection &&other) noexcept
    {
        if (this != &other) {
            reset();
            state_ = other.state_;
            other.state_ = nullptr;
        }
        return *this;
    }

    ~callback_connection() { reset(); }

    static callback_connection connect(widget_ref widget, const char *resource,
                                       callback function,
                                       error_sink sink = error_sink())
    {
        if (!widget || resource == nullptr || function == nullptr)
            throw std::invalid_argument(
                "callback connection requires a widget, resource and callback");

        state *value = new state(widget.get(), resource, std::move(function),
                                 std::move(sink));
        XtAddCallback(value->widget, value->resource.c_str(), &invoke, value);
        XtAddCallback(value->widget, XtNdestroyCallback, &destroyed, value);
        return callback_connection(value);
    }

    /* Safe to call from a callback.  Final deletion is deferred until the
     * active trampoline has returned to Xt. */
    void reset() noexcept
    {
        state *value = state_;
        if (value == nullptr)
            return;
        state_ = nullptr;
        value->detached = true;
        if (value->widget != nullptr && !value->destroyed) {
            XtRemoveCallback(value->widget, value->resource.c_str(), &invoke, value);
            XtRemoveCallback(value->widget, XtNdestroyCallback, &destroyed, value);
        }
        release(value);
    }

    bool connected() const noexcept
    {
        return state_ != nullptr && !state_->detached && !state_->destroyed;
    }

private:
    struct state {
        state(Widget value, const char *name, callback callback_value,
              error_sink sink_value)
            : widget(value), resource(name), function(std::move(callback_value)),
              sink(std::move(sink_value))
        {
        }

        Widget widget;
        std::string resource;
        callback function;
        error_sink sink;
        std::size_t active = 0;
        bool detached = false;
        bool destroyed = false;
    };

    explicit callback_connection(state *value) noexcept : state_(value) {}

    static void release(state *value) noexcept
    {
        if (value->detached && value->active == 0)
            delete value;
    }

    static void invoke(Widget widget, XtPointer closure, XtPointer call_data) noexcept
    {
        state *value = static_cast<state *>(closure);
        if (value == nullptr || value->detached || value->destroyed)
            return;
        ++value->active;
        try {
            value->function(widget_ref(widget), call_data);
        } catch (...) {
            if (value->sink != nullptr) {
                try {
                    value->sink(std::current_exception());
                } catch (...) {
                    /* Error sinks are also behind the C ABI barrier. */
                }
            }
        }
        --value->active;
        release(value);
    }

    static void destroyed(Widget, XtPointer closure, XtPointer) noexcept
    {
        state *value = static_cast<state *>(closure);
        if (value != nullptr) {
            value->destroyed = true;
            value->widget = nullptr;
        }
    }

    state *state_ = nullptr;
};

} // namespace motif

#endif
