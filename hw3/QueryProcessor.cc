/*
 * Copyright 2014 Zhuonan Sun CSE333
 * 8/9/2014
 * 1130849
 * szn1992@cs.washington.edu
 */

/*
 * Copyright 2011 Steven Gribble
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

#include <iostream>
#include <algorithm>

#include "./QueryProcessor.h"

extern "C" {
  #include "./libhw1/CSE333.h"
}

namespace hw3 {

QueryProcessor::QueryProcessor(list<string> indexlist, bool validate) {
  // Stash away a copy of the index list.
  indexlist_ = indexlist;
  arraylen_ = indexlist_.size();
  Verify333(arraylen_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader *[arraylen_];
  itr_array_ = new IndexTableReader *[arraylen_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::iterator idx_iterator = indexlist_.begin();
  for (HWSize_t i = 0; i < arraylen_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = new DocTableReader(fir.GetDocTableReader());
    itr_array_[i] = new IndexTableReader(fir.GetIndexTableReader());
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (HWSize_t i = 0; i < arraylen_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}

// Helper method of ProcessQuery to return a vector of QueryResults,
// sorted in descending order
// an empty vector will be returned, if no document matches
static vector<QueryProcessor::QueryResult>
             lookupWord(DocTableReader **dtr_array_,
             IndexTableReader **itr_array_,
             HWSize_t arraylen_,
             const string word) {
  vector<QueryProcessor::QueryResult> res;
  // iterator of indices
  for (HWSize_t j = 0; j < arraylen_; j++) {
    DocIDTableReader *reader =
    (itr_array_[j])->LookupWord(word);

    // goes to the next index, if curr index has no matches
    if (reader == NULL) {
      continue;
    } else {
      // documents where matches are found
      list<docid_element_header> docid_list =
      reader->GetDocIDList();

      // gets an QueryResult object for each doc   
      while (!docid_list.empty()) {
        docid_element_header header = docid_list.front();
        docid_list.pop_front();
        // j DocTableReader in dtr_array_
        string docname;
        if (dtr_array_[j]->LookupDocID(header.docid, &docname)) {
          QueryProcessor::QueryResult qr;
          qr.document_name = docname;
          qr.rank = header.num_positions;
          res.push_back(qr);
        }
      }
    }
    delete reader;
  }
  return res;
}

vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string> &query) {
  Verify333(query.size() > 0);
  vector<QueryProcessor::QueryResult> finalresult;

  // MISSING:
  if (query.size() == 0)
    return finalresult;  // returns an empty vector if no word's in query

  // search the first word
  string first = query.front();
  vector<QueryProcessor::QueryResult> firstresult =
    lookupWord(dtr_array_, itr_array_, arraylen_, first);

  if (firstresult.empty())
    return finalresult;  // returns an empty vector if no document matches

  if (query.size() > 1) {
    // get an iterator of the vector of query
    auto it = query.begin(); it++;
    for (; it != query.end(); it++) {
      vector<QueryProcessor::QueryResult> res =
      lookupWord(dtr_array_, itr_array_, arraylen_, *it);

      if (res.empty())
        return finalresult;  // returns an empty vector if no document matches

      // check firstresult
      // if docid exists in the current res, merge the rank
      // otherwise delete the element
      for (auto it2 = firstresult.begin(); it2 != firstresult.end();) {
        HWSize_t newrank = 0;
        bool found = false;
        for (auto it3 = res.begin(); it3 != res.end(); it3++) {
          // check if docname is found in the vector
          // gets its rank into newrank
          if (it2->document_name.compare(it3->document_name) == 0) {
            newrank = it3->rank;
            found = true;
          }
        }

        if (found) {
          it2->rank += newrank;
          it2++;
        } else {
          firstresult.erase(it2);
          if (firstresult.size() == 0)
            return finalresult;
        }
      }
    }
  }
  finalresult = firstresult;

  // Sort the final results.
  std::sort(finalresult.begin(), finalresult.end());
  return finalresult;
}

}  // namespace hw3

