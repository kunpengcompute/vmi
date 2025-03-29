#!/bin/bash

docker build -t input-device-permission:1.0 .
docker save input-device-permission:1.0 -o input-device-permission.tar
crictl rmi docker.io/library/input-device-permission:1.0
ctr -n k8s.io images import input-device-permission.tar

