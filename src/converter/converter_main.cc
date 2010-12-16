// Copyright 2010, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "base/base.h"
#include "base/util.h"
#include "converter/converter_interface.h"
#include "converter/segments.h"

namespace {
bool ExecCommand(const mozc::ConverterInterface &converter,
                 mozc::Segments *segments,
                 const string &line) {
  vector<string> fields;
  mozc::Util::SplitStringUsing(line, "\t ", &fields);

#define CHECK_FIELDS_LENGTH(length) \
  if (fields.size() < (length)) { \
     return false; \
  }

  CHECK_FIELDS_LENGTH(1);

  const string &func = fields[0];

  if (func == "startconversion" || func == "start" || func == "s") {
    CHECK_FIELDS_LENGTH(2);
    return converter.StartConversion(segments, fields[1]);
  } else if (func == "reverseconversion" || func == "reverse" || func == "r") {
    CHECK_FIELDS_LENGTH(2);
    return converter.StartReverseConversion(segments, fields[1]);
  } else if (func == "startprediction" || func == "predict" || func == "p") {
    CHECK_FIELDS_LENGTH(2);
    return converter.StartPrediction(segments, fields[1]);
  } else if (func == "startsuggestion" || func == "suggest") {
    CHECK_FIELDS_LENGTH(2);
    return converter.StartSuggestion(segments, fields[1]);
  } else if (func == "finishconversion" || func == "finish") {
    return converter.FinishConversion(segments);
  } else if (func == "resetconversion" || func == "reset") {
    return converter.ResetConversion(segments);
  } else if (func == "cancelconversion" || func == "cancel") {
    return converter.CancelConversion(segments);
  } else if (func == "getcandidates" || func == "get") {
    CHECK_FIELDS_LENGTH(3);
    return converter.GetCandidates(segments,
                                   atoi32(fields[1].c_str()),
                                   atoi32(fields[2].c_str()));
  } else if (func == "commitsegmentvalue" || func == "commit") {
    CHECK_FIELDS_LENGTH(3);
    return converter.CommitSegmentValue(segments,
                                        atoi32(fields[1].c_str()),
                                        atoi32(fields[2].c_str()));
  } else if (func == "focussegmentvalue" || func == "focus") {
    CHECK_FIELDS_LENGTH(3);
    return converter.FocusSegmentValue(segments,
                                       atoi32(fields[1].c_str()),
                                       atoi32(fields[2].c_str()));
  } else if (func == "submitfirstsegment") {
    CHECK_FIELDS_LENGTH(2);
    return converter.SubmitFirstSegment(segments,
                                        atoi32(fields[1].c_str()));

  } else if (func == "freesegmentvalue" || func == "free") {
    CHECK_FIELDS_LENGTH(2);
    return converter.FreeSegmentValue(segments,
                                      atoi32(fields[1].c_str()));
  } else if (func == "resizesegment" || func == "resize") {
    if (fields.size() == 3) {
      return converter.ResizeSegment(segments,
                                     atoi32(fields[1].c_str()),
                                     atoi32(fields[2].c_str()));
    } else if (fields.size() > 3) {
      vector<uint8> new_arrays;
      for (size_t i = 3; i < fields.size(); ++i) {
        new_arrays.push_back(static_cast<uint8>(atoi32(fields[i].c_str())));
      }
      return converter.ResizeSegment(segments,
                                     atoi32(fields[1].c_str()),  // start
                                     atoi32(fields[2].c_str()),
                                     &new_arrays[0],
                                     new_arrays.size());
    }
  } else if (func == "disableuserhistory") {
    segments->disable_user_history();
  } else if (func == "enableuserhistory") {
    segments->enable_user_history();
  } else {
    LOG(WARNING) << "Unknown command: " <<  func;
    return false;
  }

#undef CHECK_FIELDS_LENGTH
  return true;
}
}   // namespace

int main(int argc, char **argv) {
  InitGoogle(argv[0], &argc, &argv, false);
  mozc::ConverterInterface *converter
      = mozc::ConverterFactory::GetConverter();
  CHECK(converter);

  mozc::Segments segments;
  string line;

  while (getline(cin, line)) {
    string tmp;
    if (ExecCommand(*converter, &segments, line)) {
      string output;
      cout << segments.DebugString();
    } else {
      cout << "ExecCommand() return false" << endl;
    }
  }
  return 0;
}
