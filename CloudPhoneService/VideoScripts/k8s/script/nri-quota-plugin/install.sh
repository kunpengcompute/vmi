#!/bin/bash
# Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

echo "安装NRI配额插件..."

mkdir -p /opt/nri-quota-plugin
mkdir -p /var/log/nri
mkdir -p /var/run/nri

cp quota-plugin /opt/nri-quota-plugin/
chmod +x /opt/nri-quota-plugin/quota-plugin

cp quota-plugin.service /etc/systemd/system/

systemctl daemon-reload
systemctl enable quota-plugin
systemctl start quota-plugin

echo "安装完成！"
echo "检查插件状态:"
systemctl status quota-plugin --no-pager

echo ""
echo "查看日志:"
tail -20 /var/log/nri/quota-plugin.log