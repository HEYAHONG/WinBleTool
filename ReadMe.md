# ***仅供测试***

# 说明

这是一个利用WinBle库在Windows 10及以上系统获取BLE设备信息的工具。

## 使用条件

- Windows 10以上系统
- BLE设备已于Windows配对。（见[WinBle](https://github.com/DerekGn/WinBle.git)）

## 源代码下载

由于本源代码包含第三方源代码,故直接下载可能有部分源代码缺失，需要通过以下方法解决:

- 在进行git clone 使用--recurse-submodules参数。

- 若已通过git clone下载,则在源代码目录中执行以下命令下载子模块:

  ```bash
   git submodule update --init --recursive
  ```
# 编译

采用CMAKE编译,理论上只要安装好wxWidgets即可编译。

主要支持与测试以下编译环境:


- MSYS2 Mingw32

  ```bash
  #安装相应工具及库
  pacman -S mingw-w64-i686-cmake mingw-w64-i686-ninja mingw-w64-i686-make mingw-w64-i686-pkgconf mingw-w64-i686-toolchain mingw-w64-i686-wxWidgets 
  #创建目录
  mkdir build
  #进入刚才的目录并使用CMake生成工程
  cd build 
  cmake ../
  #编译(默认生成ninja工程使用ninja编译,若默认生成make工程则使用mingw32-make编译)
  ninja #ninja工程
  mingw32-make #make工程
  ```