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
