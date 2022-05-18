#ifndef _RS_LOG_H
#define _RS_LOG_H

#define LOG_LOCAL_LEVEL  5 //CONFIG_LOG_MAXIMUM_LEVEL

typedef enum {
    LOG_NONE,       /*!< No log output */
    LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    LOG_INFO,       /*!< Information messages which describe normal flow of events */
    LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output
. */
} RsLogLevel_t;

#define LOGE( tag, format, ... ) LOG_LEVEL_LOCAL(LOG_ERROR,   tag, format, ##__VA_ARGS__)
#define LOGW( tag, format, ... ) LOG_LEVEL_LOCAL(LOG_WARN,    tag, format, ##__VA_ARGS__)
#define LOGI( tag, format, ... ) LOG_LEVEL_LOCAL(LOG_INFO,    tag, format, ##__VA_ARGS__)
#define LOGD( tag, format, ... ) LOG_LEVEL_LOCAL(LOG_DEBUG,   tag, format, ##__VA_ARGS__)
#define LOGV( tag, format, ... ) LOG_LEVEL_LOCAL(LOG_VERBOSE, tag, format, ##__VA_ARGS__)

#define LOG_LEVEL(level, tag, format, ...) 

/* #define LOG_LEVEL(level, tag, format, ...) do {                     \ */
/*         if (level==ESP_LOG_ERROR )          { esp_log_write(ESP_LOG_ERROR,      tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \ */
/*         else if (level==ESP_LOG_WARN )      { esp_log_write(ESP_LOG_WARN,       tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \ */
/*         else if (level==ESP_LOG_DEBUG )     { esp_log_write(ESP_LOG_DEBUG,      tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \ */
/*         else if (level==ESP_LOG_VERBOSE )   { esp_log_write(ESP_LOG_VERBOSE,    tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \ */
/*         else                                { esp_log_write(ESP_LOG_INFO,       tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag __VA_OPT__(,) __VA_ARGS__); } \ */
/*     } while(0) */

#define LOG_LEVEL_LOCAL(level, tag, format, ...) do {               \
        if ( LOG_LOCAL_LEVEL >= level ) LOG_LEVEL(level, tag, format, ##__VA_ARGS__); \
    } while(0)


#endif // _RS_LOG_H
