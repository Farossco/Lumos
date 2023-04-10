#ifndef UTILS_C_H
#define UTILS_C_H

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))
#define RGB_BLACK (rgb_t) { 0 }
#define RGB_RED   (rgb_t) { 255, 0, 0 }
#define RGB_GREEN (rgb_t) { 0, 255, 0 }
#define RGB_BLUE  (rgb_t) { 0, 0, 255 }
#define RGB_WHITE (rgb_t) { 255, 255, 255 }

extern char err_buf[50];

#define err2str(err) (esp_err_to_name_r(err, err_buf, sizeof(err_buf)))

#endif /* ifndef UTILS_C_H */
