//===------------------------- future.cpp ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <tr3/future>

_LIBCPP_BEGIN_NAMESPACE_TR3

future<void>::future(__assoc_sub_state* __state)
    : __state_(__state)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    if (__state_->__has_future_attached())
        throw future_error(make_error_code(future_errc::future_already_retrieved));
#endif
    __state_->__add_shared();
    __state_->__set_future_attached();
}

future<void>::~future()
{
    if (__state_)
        __state_->__release_shared();
}

void
future<void>::get()
{
    unique_ptr<__shared_count, __release_shared_count> __(__state_);
    __assoc_sub_state* __s = __state_;
    __state_ = nullptr;
    __s->copy();
}

promise<void>::promise()
    : __state_(new __assoc_sub_state)
{
}

promise<void>::~promise()
{
    if (__state_)
    {
        if (!__state_->__has_value() && __state_->use_count() > 1)
            __state_->set_exception(make_exception_ptr(
                      future_error(make_error_code(future_errc::broken_promise))
                                                      ));
        __state_->__release_shared();
    }
}

future<void>
promise<void>::get_future()
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    if (__state_ == nullptr)
        throw future_error(make_error_code(future_errc::no_state));
#endif
    return future<void>(__state_);
}

void
promise<void>::set_value()
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    if (__state_ == nullptr)
        throw future_error(make_error_code(future_errc::no_state));
#endif
    __state_->set_value();
}

void
promise<void>::set_exception(exception_ptr __p)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    if (__state_ == nullptr)
        throw future_error(make_error_code(future_errc::no_state));
#endif
    __state_->set_exception(__p);
}

void
promise<void>::set_value_at_thread_exit()
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    if (__state_ == nullptr)
        throw future_error(make_error_code(future_errc::no_state));
#endif
    __state_->set_value_at_thread_exit();
}

void
promise<void>::set_exception_at_thread_exit(exception_ptr __p)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    if (__state_ == nullptr)
        throw future_error(make_error_code(future_errc::no_state));
#endif
    __state_->set_exception_at_thread_exit(__p);
}

shared_future<void>::~shared_future()
{
    if (__state_)
        __state_->__release_shared();
}

shared_future<void>&
shared_future<void>::operator=(const shared_future& __rhs)
{
    if (__rhs.__state_)
        __rhs.__state_->__add_shared();
    if (__state_)
        __state_->__release_shared();
    __state_ = __rhs.__state_;
    return *this;
}

future<void>
make_ready_future()
{
    promise<void> p;
    p.set_value();
    return p.get_future();
}

void
__assoc_sub_state::set_continuation(unique_ptr<__sub_cont> __con)
{
    __continuation_ = std::move(__con);
}

void
__sub_cont::__on_zero_shared() _NOEXCEPT
{
    delete this;
}

_LIBCPP_END_NAMESPACE_TR3

