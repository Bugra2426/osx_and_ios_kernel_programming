#!/bin/bash

# Check if running with root privileges
if [[ $EUID -ne 0 ]]; then
    echo "Please run this script as root."
    exit 1
fi

# Check if enough arguments are provided
if [[ $# -lt 2 ]]; then
    echo "Usage: sudo ./kext_script.sh <load|unload> <kext_path>"
    exit 1
fi

# Set the action (load/unload)
action="$1"

# Set the kext path
kext_path="$2"

# Check if the specified kext exists
if [[ ! -d "$kext_path" ]]; then
    echo "Invalid kext path. Please provide the correct path to the kext."
    exit 1
fi


# Set the correct ownership and permissions for the kext
chown -R root:wheel "$kext_path"
chmod -R 755 "$kext_path"

# Perform the specified action (load/unload)
if [[ "$action" == "load" ]]; then
    # Load the kext
    kextload "$kext_path"
    exit $?
elif [[ "$action" == "unload" ]]; then
    # Unload the kext
    kextunload "$kext_path"
    exit $?
else
    echo "Invalid action. Please specify 'load' or 'unload'."
    exit 1
fi

