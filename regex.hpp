#ifndef __SPAZZ_REGEX_HEADER__
#define __SPAZZ_REGEX_HEADER__
#include <iostream>
#include <regex>
#include <map>

void print_regex_error(std::regex_constants::error_type a);
#define REQUEST_TYPE_GET 1
#define REQUEST_TYPE_POST 2

int parse_headers(std::string headers);

#endif
