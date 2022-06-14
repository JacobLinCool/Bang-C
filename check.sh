#!/bin/sh
ENV=0
until ./main > 1.out; [ $? -eq 139 ]; do
    ENV=`expr $ENV + 1`
    echo $ENV
    sleep 1
done