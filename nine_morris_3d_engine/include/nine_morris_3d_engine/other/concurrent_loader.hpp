// #pragma once

// #include <functional>
// #include <atomic>
// #include <thread>

// #include "nine_morris_3d_engine/application/context.hpp"
// #include "nine_morris_3d_engine/other/resources_cache.hpp"

// namespace sm {
//     /*
//         Objects of this class load assets in a separate thread.
//         Inherit this and implement a loading function to load the assets.
//     */
//     template<typename D, typename... Args>
//     class ConcurrentLoader {
//     public:
//         using Callback = std::function<void()>;

//         explicit ConcurrentLoader(const Callback& callback_function)
//             : callback_function(callback_function) {}
//         virtual ~ConcurrentLoader();

//         ConcurrentLoader(const ConcurrentLoader&) = delete;
//         ConcurrentLoader& operator=(const ConcurrentLoader&) = delete;
//         ConcurrentLoader(ConcurrentLoader&&) = delete;
//         ConcurrentLoader& operator=(ConcurrentLoader&&) = delete;

//         void start_loading_thread(const Args&... args);
//         void update(Ctx* ctx);
//         bool is_done() const;
//         bool is_in_use() const;
//         void join_and_merge(ResourcesCache& res);
//         void join();
//     protected:
//         void set_done();

//         ResourcesCache res;
//     private:
//         void reset();
//         void join_thread();

//         Callback callback_function;
//         std::thread loading_thread;
//         std::atomic<bool> loaded {false};
//         bool in_use {false};
//     };

//     template<typename D, typename... Args>
//     ConcurrentLoader<D, Args...>::~ConcurrentLoader() {
//         join_thread();
//     }

//     template<typename D, typename... Args>
//     void ConcurrentLoader<D, Args...>::start_loading_thread(const Args&... args) {
//         in_use = true;
//         loading_thread = std::thread([&]() {
//             static_cast<D*>(this)->load(args...);
//         });
//     }

//     template<typename D, typename... Args>
//     void ConcurrentLoader<D, Args...>::update(Ctx* ctx) {
//         if (in_use && is_done()) {
//             join_and_merge(ctx->res);
//             reset();

//             callback_function();
//         }
//     }

//     template<typename D, typename... Args>
//     bool ConcurrentLoader<D, Args...>::is_done() const {
//         return loaded.load();
//     }

//     template<typename D, typename... Args>
//     bool ConcurrentLoader<D, Args...>::is_in_use() const {
//         return in_use;
//     }

//     template<typename D, typename... Args>
//     void ConcurrentLoader<D, Args...>::join_and_merge(ResourcesCache& res) {
//         loading_thread.join();

//         res.merge(this->res);
//     }

//     template<typename D, typename... Args>
//     void ConcurrentLoader<D, Args...>::join() {
//         join_thread();
//     }

//     template<typename D, typename... Args>
//     void ConcurrentLoader<D, Args...>::set_done() {
//         loaded.store(true);
//     }

//     template<typename D, typename... Args>
//     void ConcurrentLoader<D, Args...>::reset() {
//         if (loading_thread.joinable()) {
//             loading_thread.join();
//         }

//         res.clear();
//         loaded.store(false);
//         in_use = false;
//     }

//     template<typename D, typename... Args>
//     void ConcurrentLoader<D, Args...>::join_thread() {
//         loading_thread.join();
//     }
// }
