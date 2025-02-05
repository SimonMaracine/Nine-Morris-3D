#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wconversion"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4245)
    #pragma warning(disable: 4100)
    #pragma warning(disable: 4127)
#endif

// https://github.com/chriskohlhoff/asio/issues/1441

#include <boost/asio.hpp>
#define BOOST_PROCESS_VERSION 2
#include <boost/process.hpp>

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
