/*
 * Copyright 2014 Zhuonan Sun CSE333
 * 8/21/2014
 * 1130849
 * szn1992@cs.washington.edu
 */

/*
 * Copyright 2012 Steven Gribble
 *
 *  This file is part of the UW CSE 333 course project sequence
 *  (333proj).
 *
 *  333proj is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  333proj is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with 333proj.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

using std::map;
using std::string;
using namespace boost;

namespace hw4 {

bool HttpConnection::GetNextRequest(HttpRequest *request) {
  // Use "WrappedRead" to read data into the buffer_
  // instance variable.  Keep reading data until either the
  // connection drops or you see a "\r\n\r\n" that demarcates
  // the end of the request header.
  //
  // Once you've seen the request header, use ParseRequest()
  // to parse the header into the *request argument.
  //
  // Very tricky part:  clients can send back-to-back requests
  // on the same socket.  So, you need to preserve everything
  // after the "\r\n\r\n" in buffer_ for the next time the
  // caller invokes GetNextRequest()!

  // MISSING:
  while (1) {
    size_t end_of_header = buffer_.find("\r\n\r\n") + 3;
    // checks if buffer_ has the end of request header
    if (end_of_header >= 3) {
      // parse the end of request header
      *request = ParseRequest(end_of_header);
      size_t leftover = buffer_.length() - end_of_header - 1;
      buffer_ = buffer_.substr(end_of_header + 1, leftover);
      return true;
    }

    // there is more to read
    unsigned char buf[1024];
    int res = WrappedRead(fd_, buf, 1024);

    if (res <= 0) {
      break;  // loses connection
    }

    // copy read into buffer_
    buf[res] = '\0';
    buffer_ += reinterpret_cast<char*>(buf);
  }
  return false;
}

bool HttpConnection::WriteResponse(const HttpResponse &response) {
  std::string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         (unsigned char *) str.c_str(),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(size_t end) {
  HttpRequest req;
  req.URI = "/";  // by default, get "/".

  // Get the header.
  std::string str = buffer_.substr(0, end);

  // Split the header into lines.  Extract the URI from the first line
  // and store it in req.URI.  For each additional line beyond the
  // first, extract out the header name and value and store them in
  // req.headers (i.e., req.headers[headername] = headervalue).
  // You should look at HttpResponse.h for details about the HTTP header
  // format that you need to parse.
  //
  // You'll probably want to look up boost functions for (a) splitting
  // a string into lines on a "\r\n" delimiter, (b) trimming
  // whitespace from the end of a string, and (c) converting a string
  // to lowercase.

  // MISSING:
  std::vector <std::string> lines;

  // split the header into lines
  split(lines, str, is_any_of("\r\n"));
  for (auto it = lines.begin(); it != lines.end(); it++) {
    trim(*it);
  }

  std::vector <std::string> tokens;
  std::string firstline = lines.front();
  lines.erase(lines.begin());

  // extract the URI from the first line and store it in req.URI
  split(tokens, firstline, is_any_of(" "));
  req.URI = tokens.at(1);

  for (auto it = lines.begin(); it != lines.end(); it++) {
    to_lower(*it);
    std::vector <std::string> name_value;

    // extract out the header name and value and store them in req.headers
    split(name_value, *it, is_any_of(": "));
    req.headers[name_value.front()] = name_value.back();
  }

  return req;
}

}  // namespace hw4
