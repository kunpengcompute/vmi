#!/bin/bash

echo "编译NRI配额插件..."
go build -o quota-plugin main.go
chmod +x quota-plugin

echo "编译完成: quota-plugin"
ls -lh quota-plugin