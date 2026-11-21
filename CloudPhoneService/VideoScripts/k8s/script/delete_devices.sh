# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# vmi is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

#/bin/bash
export HostDevicesImageName=docker.io/library/k8s-hostdev-plugin:0.1
kubectl delete -f binder.yaml
kubectl delete -f hwbinder.yaml
kubectl delete -f vndbinder.yaml
kubectl delete -f ashmem.yaml
kubectl delete -f fuse.yaml
kubectl delete -f ion.yaml
kubectl delete -f uinput.yaml
kubectl delete -f input.yaml
kubectl delete -f exagear.yaml
kubectl delete -f host-device-plugin-ashmem.yaml
kubectl delete -f host-device-plugin-binder.yaml
kubectl delete -f host-device-plugin-hwbinder.yaml
kubectl delete -f host-device-plugin-ion.yaml
kubectl delete -f host-device-plugin-uinput.yaml
kubectl delete -f host-device-plugin-vndbinder.yaml
kubectl delete -f host-device-plugin_fuse.yaml
kubectl delete -f host-device-plugin-input.yaml
kubectl delete -f host-device-plugin-exagear.yaml
