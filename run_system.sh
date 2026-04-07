#!/bin/bash

EXECUTABLE=monitor
DIRECTORY=$1
LOGFILE=logs/report.txt
#create logs dir if missing
mkdir -p logs

#Check if directory argument is given
if [ -z "$DIRECTORY" ]; then
    echo "Error: Please provide a directory"
    echo "Usage: ./run_system.sh <directory>"
    exit 1
fi

echo "Compiling project..."
make

#check if make was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

echo "Running program..."

# redirect output to log file
./$EXECUTABLE "$DIRECTORY" &>> "$LOGFILE"

# Check if program ran successfully

if [ $? -eq 0 ]; then
    echo "Script executed successfully"
    echo "Check logs in $LOGFILE"
else
    echo "Execution failed"
fi
