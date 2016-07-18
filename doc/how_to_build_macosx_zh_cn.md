# 架设 MacOSX 的开发环境

## 必须的软件

- git
- brew
- Xcode
- cmake
  - Version 3.2.2 已确定可用。
- Qt
  - http://download.qt.io/official_releases/qt/5.6/5.6.0/
    - qt-opensource-mac-x64-clang-5.6.0.dmg
- boost
  - http://www.boost.org/users/history/version_1_55_0.html

## 在 MacOSX 中生成

### 0. 安装 Qt

### 1. 安装依赖包

可以使用下列命令调用 homebrew 进行安装

```
$ brew install glew lz4 libjpeg libpng lzo pkg-config libusb
```

或者你也可以手动生成和安装它们


### 2. Clone 存储库

```
$ git clone https://github.com/opentoonz/opentoonz
```

### (可选) 创建 stuff 目录

如果 `/Applications/OpenToonz/OpenToonz_1.0_stuff` 这个目录不存在，必须输入下列的命令。

```
$ sudo cp -r opentoonz/stuff /Applications/OpenToonz/OpenToonz_1.0_stuff
```

### 3. 生成 thirdparty 下的 tiff

```
$ cd opentoonz/thirdparty/tiff-4.0.3
$ ./configure && make
```

### 4. 将 Boost 库放入到 thirdpaty 目录
下列命令将假设 `boost_1_55_0.tar.bz2` 是下载到  `~/Downloads`，如果不是，请自己修改命令。

```
$ cd ../boost
$ mv ~/Downloads/boost_1_55_0.tar.bz2 .
$ tar xjvf boost_1_55_0.tar.bz2
```

### 5. 生成主程序

```
$ cd ../../toonz
$ mkdir build
$ cd build
  CMAKE_PREFIX_PATH=~/Qt5.6.0/5.6/clang_64 cmake ../sources
$ make
```

这将需要一段时间，请耐心等候。

### 生成之后

```
$ open ./toonz/OpenToonz_1.0.app
```
