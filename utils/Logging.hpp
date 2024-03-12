#ifndef LOGGING_HPP
#define LOGGING_HPP

#define LOG_ERR(...) fprintf(stderr, __VA_ARGS__)
#define LOG(...) fprintf(stdout, __VA_ARGS__)

#endif