#include "regex.hpp"

void print_regex_error(std::regex_constants::error_type a){
if( a == std::regex_constants::error_collate ){
    std::cout <<"The expression contained an invalid collating element name\n";
}
if( a == std::regex_constants::error_ctype ){
    std::cout <<"The expression contained an invalid character class name.\n";
}
if( a == std::regex_constants::error_escape ){
    std::cout << "The expression contained an invalid escaped character, or a trailing escape.\n";
}
if( a == std::regex_constants::error_backref ){
    std::cout <<"The expression contained an invalid back reference.\n";
}
 if( a == std::regex_constants::error_brack ){
    std::cout <<"The expression contained mismatched brackets ([ and ]).\n";
}
if( a == std::regex_constants::error_paren ){
    std::cout <<"The expression contained mismatched parentheses (( and )).\n";
}
if( a == std::regex_constants::error_brace ){
    std::cout << "The expression contained mismatched braces ({ and }).\n";
}
if( a == std::regex_constants::error_badbrace ){
    std::cout << "The expression contained an invalid range between braces ({ and }).\n";
}
if( a == std::regex_constants::error_range ){
    std::cout << "The expression contained an invalid character range.\n";
}
if( a == std::regex_constants::error_space ){
    std::cout << "There was insufficient memory to convert the expression into a finite state machine.\n";
}
if( a == std::regex_constants::error_badrepeat ){
    std::cout <<"The expression contained a repeat specifier (one of *?+{) that was not preceded by a valid regular expression.\n";
}
if( a == std::regex_constants::error_complexity ){
    std::cout <<"The complexity of an attempted match against a regular expression exceeded a pre-set level.\n";
}
if( a == std::regex_constants::error_stack ){
    std::cout << "There was insufficient memory to determine whether the regular expression could match the specified character sequence.\n";
}
}

int parse_headers(std::string headers){
try{
    std::regex p ("([A-Z]{3,4}) ([^ ]+) HTTP/1.1",std::regex::extended);
std::cout << "after\n";
    std::smatch matches;
    std::cout << "Matching...\n";
    if( regex_search(headers,matches,p) ){
        std::cout << "Matched: " << matches[0] << "\n";
        if( 1 < matches.size() && matches[1].matched ){
            std::cout << "matched1: " << matches[1] << "\n";
        }
    }else{
        std::cout << "No match\n";
    }
}catch(std::regex_error a){
    std::cout << "Regex exception: " << a.what() << "\n";
    print_regex_error(a.code());
}
    //TODO fix this
    return REQUEST_TYPE_GET;
}

