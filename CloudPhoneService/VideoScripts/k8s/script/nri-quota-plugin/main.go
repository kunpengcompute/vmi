package main

import (
	"context"
	"fmt"
	"log"
	"os"
	"os/exec"
	"strconv"
	"strings"
	"time"

	"github.com/containerd/nri/pkg/api"
	"github.com/containerd/nri/pkg/stub"
)

type plugin struct {
	stub.Stub
}

const (
	logFile = "/var/log/nri/quota-plugin.log"
)

func appendLog(message string) {
	f, err := os.OpenFile(logFile, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Printf("写入日志文件失败: %v", err)
		return
	}
	defer f.Close()
	timestamp := time.Now().Format("2006-01-02 15:04:05")
	f.WriteString(fmt.Sprintf("[%s] %s\n", timestamp, message))
}

func (p *plugin) PostStartContainer(ctx context.Context, pod *api.PodSandbox, container *api.Container) error {
	if pod == nil || container == nil {
		return nil
	}

	pName := pod.GetName()
	ns := pod.GetNamespace()
	cID := container.GetId()

	appendLog(fmt.Sprintf("[INFO] 检测到容器启动: Pod=%s/%s, ContainerID=%s", ns, pName, cID))

	go func(podName, namespace, containerID string) {
		appendLog(fmt.Sprintf("[INFO] 启动异步任务: Pod %s/%s, ContainerID %s", namespace, podName, containerID))

		time.Sleep(2 * time.Second)

		quotaMB := getQuotaFromAnnotation(podName, namespace)
		if quotaMB <= 0 {
			appendLog(fmt.Sprintf("[SKIP] Pod %s 无配额限制或配额值为0", podName))
			return
		}

		appendLog(fmt.Sprintf("[INFO] Pod %s 配额值: %dMB", podName, quotaMB))

		time.Sleep(500 * time.Millisecond)

		out, err := exec.Command("sh", "-c", fmt.Sprintf("mount | grep '%s.*/rootfs'", containerID)).Output()
		if err != nil {
			appendLog(fmt.Sprintf("[ERROR] 无法获取容器 %s 的挂载信息", containerID))
			return
		}

		mountInfo := string(out)
		upperDir := ""
		if strings.Contains(mountInfo, "upperdir=") {
			parts := strings.Split(mountInfo, "upperdir=")
			if len(parts) > 1 {
				upperDir = strings.Split(parts[1], ",")[0]
			}
		}

		if upperDir == "" {
			appendLog(fmt.Sprintf("[ERROR] 容器 %s 的 upperdir 为空", containerID))
			return
		}

		appendLog(fmt.Sprintf("[INFO] 容器 %s upperdir: %s", containerID, upperDir))

		hexPrefix := containerID[:8]
		projectID, err := strconv.ParseInt(hexPrefix, 16, 64)
		if err != nil {
			appendLog(fmt.Sprintf("[ERROR] 解析 ProjectID 失败: %v", err))
			return
		}

		dfOut, _ := exec.Command("df", "-T", upperDir).Output()
		dfLines := strings.Split(strings.TrimSpace(string(dfOut)), "\n")
		if len(dfLines) < 2 {
			appendLog(fmt.Sprintf("[ERROR] df输出格式异常"))
			return
		}
		fields := strings.Fields(dfLines[len(dfLines)-1])
		if len(fields) < 7 {
			appendLog(fmt.Sprintf("[ERROR] df字段数量不足"))
			return
		}
		fsType := fields[1]
		baseMount := fields[6]

		if fsType != "xfs" {
			appendLog(fmt.Sprintf("[SKIP] 容器 %s 挂载点 %s 是 %s，非 xfs，跳过配额设置", podName, baseMount, fsType))
			return
		}

		appendLog(fmt.Sprintf("[ACTION] 正在为 %s 设置 %dM 配额 (ProjectID: %v, upperdir: %s)",
			podName, quotaMB, projectID, upperDir))

		cmd1 := exec.Command("xfs_quota", "-x", "-c",
			fmt.Sprintf("project -s -p %s %v", upperDir, projectID), baseMount)
		if err := cmd1.Run(); err != nil {
			appendLog(fmt.Sprintf("[ERROR] project设置失败: %v", err))
			return
		}

		limitCmdStr := fmt.Sprintf("limit -p bsoft=%dm bhard=%dm %v", quotaMB, quotaMB, projectID)
		cmd2 := exec.Command("xfs_quota", "-x", "-c", limitCmdStr, baseMount)

		if err := cmd2.Run(); err != nil {
			appendLog(fmt.Sprintf("[ERROR] limit执行失败: %v", err))
			return
		}

		appendLog(fmt.Sprintf("[SUCCESS] Pod %s/%s 配额应用成功: %dMB", namespace, podName, quotaMB))

	}(pName, ns, cID)

	return nil
}

func getQuotaFromAnnotation(podName, namespace string) int {
	cmd := exec.Command("kubectl", "get", "pod", podName, "-n", namespace,
		"-o", "jsonpath={.metadata.annotations.system\\.size\\.mb}")
	cmd.Env = append(os.Environ(), "KUBECONFIG=/root/.kube/config")

	output, err := cmd.Output()
	if err != nil {
		appendLog(fmt.Sprintf("[WARN] 无法获取Pod %s annotation: %v", podName, err))
		return 0
	}

	quotaStr := strings.TrimSpace(string(output))
	if quotaStr == "" || quotaStr == "0" {
		return 0
	}

	quota, err := strconv.Atoi(quotaStr)
	if err != nil {
		appendLog(fmt.Sprintf("[ERROR] 配额值转换失败: %v, 原值: %s", err, quotaStr))
		return 0
	}

	return quota
}

func main() {
	os.MkdirAll("/var/log/nri", 0755)
	os.MkdirAll("/var/run/nri", 0755)

	opts := []stub.Option{
		stub.WithPluginName("quota-plugin"),
		stub.WithPluginIdx("01"),
	}

	p, err := stub.New(&plugin{}, opts...)
	if err != nil {
		log.Fatalf("创建 NRI Stub 失败: %v", err)
	}

	appendLog("[INFO] NRI 配额插件启动成功，监听中...")
	log.Println("NRI 配额插件启动成功，监听中...")

	if err := p.Run(context.Background()); err != nil {
		appendLog(fmt.Sprintf("[ERROR] 插件运行异常退出: %v", err))
		log.Fatalf("插件运行异常退出: %v", err)
	}
}