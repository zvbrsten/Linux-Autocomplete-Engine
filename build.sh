


LOG_FILE="daemon_log.txt"


if [ "$EUID" -ne 0 ]; then
  echo "Please run this script with sudo."
  exit
fi


echo "Cleaning previous build files..."
make clean > /dev/null 2>&1


echo "Building the daemon..."
make > /dev/null 2>&1

if [ ! -f "daemon" ]; then
    echo "Build failed. Check your makefile and source code for errors."
    exit
fi


echo "Starting the daemon in the background..."
nohup ./daemon > "$LOG_FILE" 2>&1 &

echo "Daemon started successfully in the background."
echo "You can check its output by tailing the log file: tail -f $LOG_FILE"
echo "To stop the daemon, use 'sudo pkill daemon'."

exit