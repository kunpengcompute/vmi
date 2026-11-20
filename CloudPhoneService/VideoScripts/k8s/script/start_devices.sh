# Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

#/bin/bash
export HostDevicesImageName=docker.io/library/k8s-hostdev-plugin:0.1
kubectl delete -f ashmem.yaml
kubectl delete -f fuse.yaml
kubectl delete -f uinput.yaml
kubectl delete -f input.yaml
kubectl delete -f exagear.yaml
kubectl delete -f host-device-plugin-ashmem.yaml
kubectl delete -f host-device-plugin-uinput.yaml
kubectl delete -f host-device-plugin_fuse.yaml
kubectl delete -f host-device-plugin-input.yaml
kubectl delete -f host-device-plugin-exagear.yaml

kubectl create -f ashmem.yaml
kubectl create -f fuse.yaml
kubectl create -f uinput.yaml
kubectl create -f input.yaml
kubectl create -f exagear.yaml
envsubst < host-device-plugin-ashmem.yaml | kubectl apply -f -
envsubst < host-device-plugin-uinput.yaml | kubectl apply -f -
envsubst < host-device-plugin_fuse.yaml | kubectl apply -f -
envsubst < host-device-plugin-input.yaml | kubectl apply -f -
envsubst < host-device-plugin-exagear.yaml | kubectl apply -f -
