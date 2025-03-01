# Environment-Monitoring-system.
A CLI-based environmental monitoring system fetching real-time weather data for Karachi
from the OpenWeather API using libcurl. The system stores data in files, generates alerts
for critical environmental conditions using Linux system calls, and demonstrates efficient
 data handling with C programming.
# Features
Real-time weather data retrieval from OpenWeather API.
Data processing and storage in raw and processed formats.
Real-time alerts for critical temperature conditions.
Modular code structure using header files for better maintainability.
Automation support with shell scripts and cron scheduling.
# Prerequisites
<!-- To run this application, you need to have the following installed: -->
# C Compiler (GCC)
# libcurl
# Linux Environment (for alerts).

# Setup on Ubuntu
1.Update your system:
## sudo apt-get update
2.Install necessary dependencies:
## sudo apt-get install gcc libcurl4-openssl-dev
3.Navigate to the project directory:
## cd Environment-Monitoring-system

*Build & Run
1.Compile the program manually:
## gcc main.c functions.c -o env_monitor -lcurl -ljansson
2.Run the program:
## ./env_monitor


# Alerts
Critical Temperature Alert: Triggered if specific critical conditions are found in the weather
data (Temperature Exceed from 20c annd Humidity Exceeds from 20).
Note: Make sure to replace the placeholder API key in functions.c with your own API key from OpenWeather.

Automating with Cron Jobs
You can schedule the application to run periodically using cron:

# Open the cron editor:
crontab -e
# Add the following line to schedule the program every hour:
0 * * * * /path/to/Environment-Monitoring-system/env_monitor
### in (path to) give you current path 

## Running in Background
You can run the application in the background using nohup:
nohup ./env_monitor &

## To stop the running process:

Get the process ID (PID):
# ps aux | grep env_monitor
Kill the process using its PID:
# kill PID

# This updated README.md is tailored to your Environmental Monitoring System project.
