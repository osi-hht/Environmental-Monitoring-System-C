
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stddef.h>

// Function prototypes
void fetch_data();
void save_data(const char *data);
void alert_if_critical(const char *data);

// Add the new function prototype
void save_raw_data(const char *data);
void save_avg_data();

#endif // FUNCTIONS_H
