//
// Created by palulukan on 6/2/20.
//

#include "StatusCodes.h"

const std::map<int, std::string> http::STATUS_CODES = {
    { 100, "Continue" },
    { 101, "Switching Protocols" },
    { 102, "Processing" },                 // RFC { 251,8, obsoleted by RFC { 491,8
    { 103, "Early Hints" },
    { 200, "OK" },
    { 201, "Created" },
    { 202, "Accepted" },
    { 203, "Non-Authoritative Information" },
    { 204, "No Content" },
    { 205, "Reset Content" },
    { 206, "Partial Content" },
    { 207, "Multi-Status" },               // RFC { 491,8
    { 208, "Already Reported" },
    { 226, "IM Used" },
    { 300, "Multiple Choices" },           // RFC { 723,1
    { 301, "Moved Permanently" },
    { 302, "Found" },
    { 303, "See Other" },
    { 304, "Not Modified" },
    { 305, "Use Proxy" },
    { 307, "Temporary Redirect" },
    { 308, "Permanent Redirect" },         // RFC { 723,8
    { 400, "Bad Request" },
    { 401, "Unauthorized" },
    { 402, "Payment Required" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
    { 405, "Method Not Allowed" },
    { 406, "Not Acceptable" },
    { 407, "Proxy Authentication Required" },
    { 408, "Request Timeout" },
    { 409, "Conflict" },
    { 410, "Gone" },
    { 411, "Length Required" },
    { 412, "Precondition Failed" },
    { 413, "Payload Too Large" },
    { 414, "URI Too Long" },
    { 415, "Unsupported Media Type" },
    { 416, "Range Not Satisfiable" },
    { 417, "Expectation Failed" },
    { 418, "I'm a Teapot" },               // RFC { 716,8
    { 421, "Misdirected Request" },
    { 422, "Unprocessable Entity" },       // RFC { 491,8
    { 423, "Locked" },                     // RFC { 491,8
    { 424, "Failed Dependency" },          // RFC { 491,8
    { 425, "Unordered Collection" },       // RFC { 491,8
    { 426, "Upgrade Required" },           // RFC { 281,7
    { 428, "Precondition Required" },      // RFC { 658,5
    { 429, "Too Many Requests" },          // RFC { 658,5
    { 431, "Request Header Fields Too Large" }, // RFC { 658,5
    { 451, "Unavailable For Legal Reasons" },
    { 500, "Internal Server Error" },
    { 501, "Not Implemented" },
    { 502, "Bad Gateway" },
    { 503, "Service Unavailable" },
    { 504, "Gateway Timeout" },
    { 505, "HTTP Version Not Supported" },
    { 506, "Variant Also Negotiates" },    // RFC { 229,5
    { 507, "Insufficient Storage" },       // RFC { 491,8
    { 508, "Loop Detected" },
    { 509, "Bandwidth Limit Exceeded" },
    { 510, "Not Extended" },               // RFC { 277,4
    { 511, "Network Authentication Required" } // RFC { 658,5
};
