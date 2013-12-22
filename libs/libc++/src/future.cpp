#include <tr3/future>

_LIBCPP_BEGIN_NAMESPACE_TR3

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

