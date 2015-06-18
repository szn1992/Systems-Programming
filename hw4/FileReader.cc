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

#include <stdio.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <memory>
#include <cstring>

extern "C" {
  #include "libhw2/fileparser.h"
}

#include "./HttpUtils.h"
#include "./FileReader.h"

namespace hw4 {

bool FileReader::ReadFile(std::string *str) {
  std::string fullfile = basedir_ + "/" + fname_;

  // Read the file into memory, and store the file contents in the
  // output parameter "str."  Be careful to handle binary data
  // correctly; i.e., you probably want to use the two-argument
  // constructor to std::string (the one that includes a length as a
  // second argument).
  //
  // You might find ::ReadFile() from HW2's fileparser.h useful
  // here.  Be careful, though; remember that it uses malloc to
  // allocate memory, so you'll need to use free() to free up that
  // memory.  Alternatively, you can use a unique_ptr with a malloc/free
  // deleter to automatically manage this for you; see the comment in
  // HttpUtils.h above the MallocDeleter class for details.

  // MISSING:
  HWSize_t len = 0;
  char *filename = new char[fullfile.size() + 1];
  memcpy(filename, fullfile.c_str(), fullfile.size() + 1);
  char *res = ::ReadFile(filename, &len);
  delete [] filename;

  if (res == NULL) {
    return false;  // fails to read the file
  }

  *str = "";
  for (HWSize_t i = 0; i < len; i++) {
    *str += res[i];
  }

  free(res);
  return true;
}

}  // namespace hw4
