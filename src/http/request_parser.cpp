//
// request_parser.cpp
// ~~~~~~~~~~~~~~~~~~
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

#include "http/request_parser.hpp"
#include "http/request.hpp"

namespace market {
    namespace http {

        request_parser::request_parser() : state_(method_start) {}

        void request_parser::reset() {
            state_ = method_start;
        }

        boost::tribool request_parser::consume(request& req, char input) {
            switch (state_) {
                case method_start:
                    if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                        return false;
                    } else {
                        state_ = method;
                        req.method.push_back(input);
                        return boost::indeterminate;
                    }
                case method:
                    if (input == ' ') {
                        state_ = uri;
                        return boost::indeterminate;
                    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                        return false;
                    } else {
                        req.method.push_back(input);
                        return boost::indeterminate;
                    }
                case uri:
                    if (input == ' ') {
                        state_ = http_version_h;
                        return boost::indeterminate;
                    } else if (is_ctl(input)) {
                        return false;
                    } else {
                        req.uri.push_back(input);
                        return boost::indeterminate;
                    }
                case http_version_h:
                    if (input == 'H') {
                        state_ = http_version_t_1;
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case http_version_t_1:
                    if (input == 'T') {
                        state_ = http_version_t_2;
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case http_version_t_2:
                    if (input == 'T') {
                        state_ = http_version_p;
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case http_version_p:
                    if (input == 'P') {
                        state_ = http_version_slash;
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case http_version_slash:
                    if (input == '/') {
                        req.http_version_major = 0;
                        req.http_version_minor = 0;
                        state_ = http_version_major_start;
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case http_version_major_start:
                    if (is_digit(input)) {
                        req.http_version_major = req.http_version_major * 10 + input - '0';
                        state_ = http_version_major;
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case http_version_major:
                    if (input == '.') {
                        state_ = http_version_minor_start;
                        return boost::indeterminate;
                    } else if (is_digit(input)) {
                        req.http_version_major = req.http_version_major * 10 + input - '0';
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case http_version_minor_start:
                    if (is_digit(input)) {
                        req.http_version_minor = req.http_version_minor * 10 + input - '0';
                        state_ = http_version_minor;
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case http_version_minor:
                    if (input == '\r') {
                        state_ = expecting_newline_1;
                        return boost::indeterminate;
                    } else if (is_digit(input)) {
                        req.http_version_minor = req.http_version_minor * 10 + input - '0';
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case expecting_newline_1:
                    if (input == '\n') {
                        state_ = header_line_start;
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case header_line_start:
                    if (input == '\r') {
                        state_ = expecting_newline_3;
                        return boost::indeterminate;
                    } else if (!req.headers.empty() && (input == ' ' || input == '\t')) {
                        state_ = header_lws;
                        return boost::indeterminate;
                    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                        return false;
                    } else {
                        req.headers.push_back(header());
                        req.headers.back().name.push_back(input);
                        state_ = header_name;
                        return boost::indeterminate;
                    }
                case header_lws:
                    if (input == '\r') {
                        state_ = expecting_newline_2;
                        return boost::indeterminate;
                    } else if (input == ' ' || input == '\t') {
                        return boost::indeterminate;
                    } else if (is_ctl(input)) {
                        return false;
                    } else {
                        state_ = header_value;
                        req.headers.back().value.push_back(input);
                        return boost::indeterminate;
                    }
                case header_name:
                    if (input == ':') {
                        state_ = space_before_header_value;
                        return boost::indeterminate;
                    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                        return false;
                    } else {
                        req.headers.back().name.push_back(input);
                        return boost::indeterminate;
                    }
                case space_before_header_value:
                    if (input == ' ') {
                        state_ = header_value;
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case header_value:
                    if (input == '\r') {
                        state_ = expecting_newline_2;
                        return boost::indeterminate;
                    } else if (is_ctl(input)) {
                        return false;
                    } else {
                        req.headers.back().value.push_back(input);
                        return boost::indeterminate;
                    }
                case expecting_newline_2:
                    if (input == '\n') {
                        state_ = header_line_start;
                        return boost::indeterminate;
                    } else {
                        return false;
                    }
                case expecting_newline_3:
                    return (input == '\n');
                default:
                    return false;
            }
        }

        bool request_parser::is_char(int c) {
            return c >= 0 && c <= 127;
        }

        bool request_parser::is_ctl(int c) {
            return (c >= 0 && c <= 31) || (c == 127);
        }

        bool request_parser::is_tspecial(int c) {
            switch (c) {
                case '(' : 
                case ')' : 
                case '<' : 
                case '>' : 
                case '@' :
                case ',' : 
                case ';' : 
                case ':' : 
                case '\\': 
                case '"' :
                case '/' :
                case '[' : 
                case ']' : 
                case '?' : 
                case '=' :
                case '{' : 
                case '}' : 
                case ' ' : 
                case '\t':
                    return true;
                default:
                    return false;
            }
        }

        bool request_parser::is_digit(int c) {
            return c >= '0' && c <= '9';
        }
    } // namespace market
} // namespace http
