//
// io_service_pool.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2013 Alex Caudill (alex at furosys.com)
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace market {
    namespace http {
        /// A pool of io_service objects.
        class io_service_pool : private boost::noncopyable {
            public:
                /// Construct the io_service pool.
                explicit io_service_pool(std::size_t pool_size);
                /// Run all io_service objects in the pool.
                void run();
                /// Stop all io_service objects in the pool.
                void stop();
                /// Get an io_service to use.
                boost::asio::io_service& get_io_service();
            private:
                typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
                typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;
                /// The pool of io_services.
                std::vector<io_service_ptr> io_services_;
                /// The work that keeps the io_services running.
                std::vector<work_ptr> work_;
                /// The next io_service to use for a connection.
                std::size_t next_io_service_;
        };
    } // namespace market
} // namespace http
