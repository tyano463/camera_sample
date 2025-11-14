# camera sample for qemu

host
```
tyano@fedora:poky$ lsusb
...
Bus 002 Device 003: ID 0c45:64d0 Microdia Integrated Webcam
...
```

yocto additional apps
```
IMAGE_INSTALL:append = " ros-core exusers openssh bash sudo rear-node v4l-utils libv4l usbutils git kmod opencv opencv-dev python3-opencv packagegroup-core-buildessential "
ROS_DISTRO = "humble"
```

qemu command
```
sudo /usr/bin/qemu-system-aarch64   -device virtio-net-pci,netdev=net0,mac=52:54:00:12:34:04   -netdev tap,id=net0,ifname=tap1,script=no,downscript=no   -object rng-random,filename=/dev/urandom,id=rng0   -device virtio-rng-pci,rng=rng0   -drive id=disk0,file=tmp/deploy/images/qemuarm64/ros-image-core-humble-qemuarm64.ext4,if=none,format=raw   -device virtio-blk-pci,drive=disk0   -device qemu-xhci -device usb-tablet -device usb-kbd   -machine virt -cpu cortex-a57 -smp 4 -m 256   -device usb-host,vendorid=0x0c45,productid=0x64d0   -serial mon:vc -serial null -display sdl,show-cursor=on   -device virtio-gpu-pci   -kernel tmp/deploy/images/qemuarm64/Image   -append 'root=/dev/vda rw  mem=256M ip=192.168.7.4::192.168.7.1:255.255.255.0::eth0:off:8.8.8.8' 
```


