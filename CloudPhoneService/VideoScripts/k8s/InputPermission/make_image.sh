#!/bin/bash
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# vmi is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

docker build -t input-device-permission:1.0 .
docker save input-device-permission:1.0 -o input-device-permission.tar
crictl rmi docker.io/library/input-device-permission:1.0
ctr -n k8s.io images import input-device-permission.tar

