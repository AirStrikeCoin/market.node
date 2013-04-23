//
// mime_types.cpp
// ~~~~~~~~~~~~~~
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

#include "http/mime_types.hpp"

namespace market {
    namespace http {
        namespace mime_types {
            struct mapping {
                const char* extension;
                const char* mime_type;
            } mappings[] = {
                { "css", "text/stylesheet" },
                { "gif", "image/gif"       },
                { "htm", "text/html"       },
                { "html", "text/html"      },
                { "jpg", "image/jpeg"      },
                { "png", "image/png"       },
                { 0, 0 } // Marks end of list.
            };

            std::string extension_to_type(const std::string& extension) {
                for (mapping* m = mappings; m->extension; ++m) {
                    if (m->extension == extension) {
                        return m->mime_type;
                    }
                }
                return "text/plain";
            }
        } // namespace mime_types
    } // namespace market
} // namespace http
