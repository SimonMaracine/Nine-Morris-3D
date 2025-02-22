#! /bin/bash

SERVICE="nine_morris_3d_server.service"
DIRECTORY="/usr/local/bin"
EXECUTABLE="nine_morris_3d_server"

systemctl disable $SERVICE

rm -v -f /etc/systemd/system/$SERVICE
rm -v -f $DIRECTORY/$EXECUTABLE
