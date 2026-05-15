#!/bin/bash

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