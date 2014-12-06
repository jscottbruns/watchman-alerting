#!/bin/bash

# Purge temp files older than 14 days
find /usr/local/watchman-alerting/tmp/* -type f -mtime +14 -exec rm '{}' \;