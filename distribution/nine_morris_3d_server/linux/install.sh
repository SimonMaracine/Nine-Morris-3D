#! /bin/bash

SERVICE="nine_morris_3d_server.service"
DIRECTORY="/usr/local/bin"
EXECUTABLE="nine_morris_3d_server"

install -v -s -T $EXECUTABLE $DIRECTORY/$EXECUTABLE
cp -v $SERVICE /etc/systemd/system/$SERVICE

systemctl enable $SERVICE
