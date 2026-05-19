# NRI Quota Plugin

轻量化NRI插件，用于自动管理Kubernetes Pod的XFS配额。

## 功能

- 监听containerd的容器启动事件
- 从Pod annotation读取`system.size.mb`配额值
- 自动应用XFS Project Quota到容器overlayfs
- 容器重启后自动重新应用配额

## 工作原理

```
容器启动 → NRI插件触发 → kubectl wait Pod Ready → 
读取annotation配额值 → 获取upperdir路径 → 
计算Project ID → 应用XFS Quota → 配额生效
```

## 构建

```bash
go build -o quota-plugin main.go
```

## 部署

### 1. 配置containerd启用NRI

编辑 `/etc/containerd/config.toml`:

```toml
[plugins."io.containerd.nri.v1.nri"]
  disable = false
  plugin_config_path = "/etc/nri"
  plugin_socket_path = "/var/run/nri"
```

重启containerd:
```bash
systemctl restart containerd
```

### 2. 部署插件

```bash
mkdir -p /var/log/nri /var/run/nri
cp quota-plugin /opt/nri-quota-plugin/
chmod +x /opt/nri-quota-plugin/quota-plugin
```

### 3. 创建systemd服务

编辑 `/etc/systemd/system/quota-plugin.service`:

```ini
[Unit]
Description=NRI Quota Plugin
After=containerd.service
Requires=containerd.service

[Service]
Type=simple
ExecStart=/opt/nri-quota-plugin/quota-plugin
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
```

启动服务:
```bash
systemctl daemon-reload
systemctl enable quota-plugin
systemctl start quota-plugin
```

## 使用

### Pod YAML配置

在Pod YAML中添加annotation:

```yaml
metadata:
  annotations:
    system.size.mb: "10240"  # 10G配额
```

### 启动Pod示例

```bash
./k8s-video.sh start 1 1 10240
```

### 验证配额

```bash
kubectl exec -it video1 -- df -h | grep '/$'
# 预期: overlay 10G

tail /var/log/nri/quota-plugin.log
# 查看配额应用日志
```

## 日志位置

- 日志文件: `/var/log/nri/quota-plugin.log`
- Socket: `/var/run/nri/quota-plugin.sock`

## 与守护进程方案对比

| 特性 | 守护进程方案 | NRI插件方案 |
|------|-------------|------------|
| 响应速度 | 3-15秒延迟 | 毫秒级实时 |
| 性能开销 | 持续轮询 | 事件驱动 |
| 集成度 | 外部进程 | 内置到containerd |
| 维护成本 | 手动管理 | systemd自动 |

## 改进点（相比原始示例）

1. **动态配额值**: 从Pod annotation读取，而非硬编码512MB
2. **通用性**: 支持任意Pod名称，不限制特定前缀
3. **日志增强**: 详细记录每个步骤
4. **错误处理**: 更完善的错误检查和日志

## 测试

```bash
# 测试首次启动
./k8s-video.sh start 1 1 10240
kubectl exec -it video1 -- df -h | grep '/$'

# 测试容器重启（关键测试）
crictl stop <container_id>
crictl rm <container_id>
sleep 20
kubectl exec -it video1 -- df -h | grep '/$'
# 预期: 仍然显示10G，配额自动重新应用
```

## 停止插件

```bash
systemctl stop quota-plugin
rm /var/run/nri/quota-plugin.sock
```