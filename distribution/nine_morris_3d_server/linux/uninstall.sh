#! /bin/bash

SERVICE="nine_morris_3d_server.service"
DIRECTORY="/usr/local/bin"
EXECUTABLE="nine_morris_3d_server"

systemctl disable $SERVICE

rm -v -f /etc/systemd/system/$SERVICE
rm -v -f $DIRECTORY/$EXECUTABLE

if [[ $1 = "all" ]]; then
    rm -v -f /etc/ninemorris3d/nine_morris_3d.json
    rmdir -v /etc/ninemorris3d

    rm -v -f /var/log/ninemorris3d/nine_morris_3d*.log
    rmdir -v /var/log/ninemorris3d
fi
