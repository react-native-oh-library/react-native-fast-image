#include <Uri.h>
#include <algorithm>
#include <cctype>
#include <boost/regex.hpp>
#include <sstream>
#include <iostream>
#include <sstream>

namespace rnoh {

namespace {

std::string submatch(const boost::cmatch& m, int idx) {
  const auto& sub = m[idx];
  return std::string(sub.first, sub.second);
}

} // namespace

Uri::Uri(folly::StringPiece str) : hasAuthority_(false), port_(0) {
  static const boost::regex uriRegex(
      "([a-zA-Z][a-zA-Z0-9+.-]*):" // scheme:
      "([^?#]*)" // authority and path
      "(?:\\?([^#]*))?" // ?query
      "(?:#(.*))?"); // #fragment
  static const boost::regex authorityAndPathRegex("//([^/]*)(/.*)?");

  boost::cmatch match;
  if (UNLIKELY(!boost::regex_match(str.begin(), str.end(), match, uriRegex))) {
        //throw std::invalid_argument(to<std::string>("invalid URI ", str));
        //not throw exception
        encodeUri_ = str;
        return;
  }
  scheme_ = submatch(match, 1);
  std::transform(scheme_.begin(), scheme_.end(), scheme_.begin(), ::tolower);

  folly::StringPiece authorityAndPath(match[2].first, match[2].second);
  if (authorityAndPath.empty()) {
    encodeUri_ = str;
    return;
  }
    
  boost::cmatch authorityAndPathMatch;
  if (!boost::regex_match(
          authorityAndPath.begin(),
          authorityAndPath.end(),
          authorityAndPathMatch,
          authorityAndPathRegex)) {
    // Does not start with //, doesn't have authority
    hasAuthority_ = false;
    path_ = authorityAndPath.str();
  } else {
    static const boost::regex authorityRegex(
        "(?:([^@:]*)(?::([^@]*))?@)?" // username, password
        "(\\[[^\\]]*\\]|[^\\[:]*)" // host (IP-literal (e.g. '['+IPv6+']',
                                   // dotted-IPv4, or named host)
        "(?::(\\d*))?"); // port

    const auto authority = authorityAndPathMatch[1];
    boost::cmatch authorityMatch;
    if (!boost::regex_match(
            authority.first,
            authority.second,
            authorityMatch,
            authorityRegex)) {
      encodeUri_ = str;
      return;
    }

    folly::StringPiece port(authorityMatch[4].first, authorityMatch[4].second);
    if (!port.empty()) {
      try {
        port_ = to<uint16_t>(port);
      } catch (folly::ConversionError const& e) {
        encodeUri_ = str;
        return;
      }
    }

    hasAuthority_ = true;
    username_ = submatch(authorityMatch, 1);
    password_ = submatch(authorityMatch, 2);
    host_ = submatch(authorityMatch, 3);
    path_ = submatch(authorityAndPathMatch, 2);
  }

  query_ = submatch(match, 3);
  fragment_ = submatch(match, 4);

  std::string subPrefix = scheme_ + "://";
  std::string source = str.str();
  try {
    encodeUri_ = subPrefix + uriEncode(source.substr(subPrefix.length(), source.length()), "@#&=*+-_.,:!?()/~'%;$");
  } catch (...) {
    encodeUri_ = str;
    return;
  }
}

std::string Uri::authority() const {
  std::string result;

  // Port is 5 characters max and we have up to 3 delimiters.
  result.reserve(host().size() + username().size() + password().size() + 8);

  if (!username().empty() || !password().empty()) {
    result.append(username());

    if (!password().empty()) {
      result.push_back(':');
      result.append(password());
    }

    result.push_back('@');
  }

  result.append(host());

  if (port() != 0) {
    result.push_back(':');
    folly::toAppend(port(), &result);
  }

  return result;
}

std::string Uri::hostname() const {
  if (!host_.empty() && host_[0] == '[') {
    // If it starts with '[', then it should end with ']', this is ensured by
    // regex
    return host_.substr(1, host_.size() - 2);
  }
  return host_;
}

const std::vector<std::pair<std::string, std::string>>& Uri::getQueryParams() {
  if (!query_.empty() && queryParams_.empty()) {
    // Parse query string
    static const boost::regex queryParamRegex(
        "(^|&)" /*start of query or start of parameter "&"*/
        "([^=&]*)=?" /*parameter name and "=" if value is expected*/
        "([^=&]*)" /*parameter value*/
        "(?=(&|$))" /*forward reference, next should be end of query or
                      start of next parameter*/);
    const boost::cregex_iterator paramBeginItr(
        query_.data(), query_.data() + query_.size(), queryParamRegex);
    boost::cregex_iterator paramEndItr;
    for (auto itr = paramBeginItr; itr != paramEndItr; ++itr) {
      if (itr->length(2) == 0) {
        // key is empty, ignore it
        continue;
      }
      queryParams_.emplace_back(
          std::string((*itr)[2].first, (*itr)[2].second), // parameter name
          std::string((*itr)[3].first, (*itr)[3].second) // parameter value
      );
    }
  }
  return queryParams_;
}

bool Uri::isAlphaNumeric(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'); }

bool Uri::isReserved(char c) { return std::string("_-!.~'()*").find(c) != std::string::npos; }

bool Uri::isAllowedEncode(char c, const std::string &allow) {
  return isAlphaNumeric(c) || isReserved(c) || (!allow.empty() && allow.find(c) != std::string::npos);
}

std::string Uri::uriEncode(const std::string &s, const std::string &allow) {
    if (s.empty()) {
        return s;
    }
    char HEX_DIGITS[] = "0123456789ABCDEF";
    std::ostringstream encoded;
    size_t oldLength = s.length();
    size_t current = 0;
    
    while (current < oldLength) {
        size_t nextToEncode = current;
        while (nextToEncode < oldLength && isAllowedEncode(s[nextToEncode], allow)) {
            nextToEncode++;
        }
        if (nextToEncode == oldLength) {
            if (current == 0) {
                return s;
            } else {
                encoded << s.substr(current, oldLength - current);
                return encoded.str();
            }
        }
        if (nextToEncode > current) {
            encoded << s.substr(current, nextToEncode - current);
        }
        current = nextToEncode;
        size_t nextAllowed = current + 1;
        while (nextAllowed < oldLength && !isAllowedEncode(s[nextAllowed], allow)) {
            nextAllowed++;
        }
        std::string toEncode = s.substr(current, nextAllowed - current);
        try {
            // Convert string to bytes using the specified encoding (assuming ASCII here)
            for (char c : toEncode) {
                encoded << '%' << HEX_DIGITS[(c & 0xf0) >> 4] << HEX_DIGITS[c & 0xf];
            }
        } catch (const std::exception &e) {
            // Handle encoding exceptions
            std::cerr << "Encoding error: " << e.what() << std::endl;
            return s;
        }

        current = nextAllowed;
    }
    return encoded.str();
}

} // namespace rnoh
