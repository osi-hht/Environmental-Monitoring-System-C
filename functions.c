
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <jansson.h>
#include <time.h>  // Include jansson for JSON parsing
#define CRITICAL_TEMP 20.0    // Temperature threshold
#define CRITICAL_HUMIDITY 10.0  // Humidity threshold

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + totalSize + 1);
    if (ptr == NULL) {
        printf("Not enough memory (realloc failed)\n");
        return 0;
    }
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, totalSize);
    mem->size += totalSize;
    mem->memory[mem->size] = 0;

    return totalSize;
}

// Function to save raw data into a file
void save_raw_data(const char *data) {
    FILE *file = fopen("raw_data.txt", "a");

    if (file == NULL) {
        fprintf(stderr, "Failed to open raw_data.txt for writing\n");
        return;
    }
    fprintf(file, "%s", data);
    fprintf(file, "\n");
       fclose(file);
    printf("Raw data saved to raw_data.txt\n");
}

void fetch_data() {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  // Initial memory allocation
    chunk.size = 0;            // Initial size set to 0

    curl = curl_easy_init();
    if (curl) {
        // Replace with the correct OpenWeather API URL for Karachi
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openweathermap.org/data/2.5/weather?q=Karachi&appid=ed039ed72698ab8834bad6053fe86478");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Data fetched: %s\n", chunk.memory);

            // Save raw data to a file
            save_raw_data(chunk.memory);

            // Existing functions to process data and handle alerts
            save_data(chunk.memory);
            alert_if_critical(chunk.memory);
        }
        curl_easy_cleanup(curl);
        free(chunk.memory);
    }
}

// Ensure to handle paths, headers, and related issues in your full implementation
// Add your other functions (save_data, alert_if_critical) below this line.

void save_data(const char *data) {
    json_error_t error;
    json_t *root = json_loads(data, 0, &error);
    if (!root) {
        printf("Error parsing JSON: %s\n", error.text);
        return;
    }

    json_t *main = json_object_get(root, "main");
    json_t *weather = json_array_get(json_object_get(root, "weather"), 0);

    if (main && weather) {
        double temp = json_number_value(json_object_get(main, "temp")) - 273.15; // Convert from Kelvin to Celsius
        double humidity = json_number_value(json_object_get(main, "humidity"));
        const char *weather_description = json_string_value(json_object_get(weather, "description"));

        // Save data to file
        FILE *file = fopen("processed_data.txt", "a");
        if (file == NULL) {
            printf("Error opening file!\n");
            return;
        }

        fprintf(file, "TEMPERATURE: %.2f°C\n", temp);
        fprintf(file, "HUMIDITY: %.2f%%\n", humidity);
        fprintf(file, "WEATHER DESCRIPTION: %s\n\n", weather_description);
        fclose(file);

        // Call the alert function if critical values are detected
        alert_if_critical(temp, humidity);
    }

    json_decref(root);
}



// Function to write the alert message with date and time to the alert file
void log_alert(const char *alert_message) {
    // Open the file for appending
    FILE *file = fopen("alerts.txt", "a");
    if (file == NULL) {
        printf("Error opening alerts.txt for writing\n");
        return;
    }

    // Get current time and date
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Format the time as "YYYY-MM-DD HH:MM:SS"
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Write the time and alert message to the file
    fprintf(file, "ALERT TIME: %s\n", time_str);
    fprintf(file, "%s\n\n", alert_message);

    // Close the file
    fclose(file);
}

void alert_if_critical(double temp, double humidity) {
    // Check if temperature exceeds critical threshold
    if (temp > CRITICAL_TEMP) {
        // Prepare the alert message
        char alert_message[256];
        snprintf(alert_message, sizeof(alert_message), "Critical Temperature Alert! Temperature: %.2f°C", temp);
        
        // Print the alert to the terminal
        printf("%s\n", alert_message);
        
        // Log the alert to the file
        log_alert(alert_message);
        
        // Send email if mail is set up
        system("echo 'Critical Temperature Alert!' | mail -s 'Alert' user@example.com");
    }

    // Check if humidity exceeds critical threshold
    if (humidity > CRITICAL_HUMIDITY) {
        // Prepare the alert message
        char alert_message[256];
        snprintf(alert_message, sizeof(alert_message), "Critical Humidity Alert! Humidity: %.2f%%", humidity);
        
        // Print the alert to the terminal
        printf("%s\n", alert_message);
        
        // Log the alert to the file
        log_alert(alert_message);
        
        // Send email if mail is set up
        system("echo 'Critical Humidity Alert!' | mail -s 'Alert' user@example.com");
    }
}



// Function to calculate and save average temperature and humidity
void save_avg_data() {
    FILE *file = fopen("processed_data.txt", "r");
    if (file == NULL) {
        printf("Error opening processed_data.txt\n");
        return;
    }

    double temp_sum = 0, humidity_sum = 0;
    int count = 0;
    double temp, humidity;

    // Read last 5 temperature and humidity values from the file
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "TEMPERATURE")) {
            sscanf(line, "TEMPERATURE: %lf", &temp);
            temp_sum += temp;
            count++;
        }
        if (strstr(line, "HUMIDITY")) {
            sscanf(line, "HUMIDITY: %lf", &humidity);
            humidity_sum += humidity;
            count++;
        }

        // Only process the last 5 readings
        if (count >= 10) break;
    }

    fclose(file);

    if (count > 0) {
        // Calculate average
        double avg_temp = temp_sum / 5;
        double avg_humidity = humidity_sum / 5;

        // Open file to save average data
        FILE *avg_file = fopen("avg_data.txt", "a");
        if (avg_file == NULL) {
            printf("Error opening avg_data.txt\n");
            return;
        }

        // Write average data to the file
        fprintf(avg_file, "AVERAGE TEMPERATURE: %.2f°C\n", avg_temp);
        fprintf(avg_file, "AVERAGE HUMIDITY: %.2f%%\n\n", avg_humidity);

        fclose(avg_file);
        printf("Average data saved to avg_data.txt\n");
    }
}
