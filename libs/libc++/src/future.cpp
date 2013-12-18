#include <tr3/future>

using namespace std::tr3;

future<void>
make_ready_future()
{
    promise<void> p;
    p.set_value();
    return p.get_future();
}



