/*
 * Copyright (c) 2003-2008, John Wiegley.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of New Artisans LLC nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TIMES_H
#define _TIMES_H

namespace ledger {

DECLARE_EXCEPTION(datetime_error, std::runtime_error);
DECLARE_EXCEPTION(date_error, std::runtime_error);

typedef boost::posix_time::ptime	datetime_t;
typedef datetime_t::time_duration_type	time_duration_t;

inline bool is_valid(const datetime_t& moment) {
  return ! moment.is_not_a_date_time();
}

typedef boost::gregorian::date          date_t;
typedef boost::gregorian::date_duration date_duration_t;

inline bool is_valid(const date_t& moment) {
  return ! moment.is_not_a_date();
}

extern const datetime_t& current_time;
extern const date_t&     current_date;
extern int		 current_year;
extern string		 input_time_format;
extern string		 output_time_format;

struct interval_t
{
  unsigned short years;
  unsigned short months;
  unsigned short days;
  date_t	 begin;
  date_t	 end;
  mutable bool   advanced;

  interval_t(int _days = 0, int _months = 0, int _years = 0,
	     const date_t& _begin = date_t(),
	     const date_t& _end   = date_t())
    : years(_years), months(_months), days(_days),
      begin(_begin), end(_end), advanced(false) {
    TRACE_CTOR(interval_t,
	       "int, int, int, const date_t&, const date_t&");
  }
  interval_t(const interval_t& other)
    : years(other.years),
      months(other.months),
      days(other.days),

      begin(other.begin),
      end(other.end),

      advanced(other.advanced) {
    TRACE_CTOR(interval_t, "copy");
  }

  interval_t(const string& desc)
    : years(0), months(0), days(0),
      begin(), end(), advanced(false) {
    TRACE_CTOR(interval_t, "const string&");
    std::istringstream stream(desc);
    parse(stream);
  }

  ~interval_t() throw() {
    TRACE_DTOR(interval_t);
  }

  operator bool() const {
    return years > 0 || months > 0  || days > 0;
  }

  void   start(const date_t& moment) {
    begin = first(moment);
  }
  date_t first(const date_t& moment = date_t()) const;
  date_t increment(const date_t&) const;

  void   parse(std::istream& in);
};

#if 0
inline datetime_t ptime_local_to_utc(const datetime_t& when) {
  struct std::tm tm_gmt = to_tm(when);
  return boost::posix_time::from_time_t(std::mktime(&tm_gmt));
}

// jww (2007-04-18): I need to make a general parsing function
// instead, and then make these into private methods.
inline datetime_t ptime_from_local_date_string(const string& date_string) {
  return ptime_local_to_utc(datetime_t(boost::gregorian::from_string(date_string),
				       time_duration()));
}

inline datetime_t ptime_from_local_time_string(const string& time_string) {
  return ptime_local_to_utc(boost::posix_time::time_from_string(time_string));
}
#endif

inline datetime_t parse_datetime(const string& str) {
  return parse_datetime(str.c_str());
}
datetime_t parse_datetime(const char * str);

inline date_t parse_date(const string& str) {
  return gregorian::from_string(str);
}

inline std::time_t to_time_t(const ptime& t) 
{ 
  if( t == posix_time::neg_infin ) 
    return 0; 
  else if( t == posix_time::pos_infin ) 
    return LONG_MAX; 
  ptime start(date(1970,1,1)); 
  return (t-start).total_seconds(); 
}

inline string format_datetime(const datetime_t& when) {
  char buf[64];
  time_t moment = to_time_t(when);
  // jww (2008-07-29): Need to make the output format configurable
  std::strftime(buf, 63, "%Y/%m/%d", std::localtime(&moment));
  return buf;
}

inline string format_date(const date_t& when) {
  return to_iso_extended_string(when);
}

#if 0
struct intorchar
{
  int	 ival;
  string sval;

  intorchar() : ival(-1) {}
  intorchar(int val) : ival(val) {}
  intorchar(const string& val) : ival(-1), sval(val) {}
  intorchar(const intorchar& o) : ival(o.ival), sval(o.sval) {}
};

ledger::datetime_t parse_abs_datetime(std::istream& input);
#endif

} // namespace ledger

#endif // _TIMES_H
