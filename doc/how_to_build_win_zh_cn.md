# 生成步骤（Windows）

本软件可以用 Visual Studio 2015 和 Qt 5.6 进行生成。

## 必需软件

### Visual Studio Express 2015 for Windows Desktop
- https://www.visualstudio.com/zh-cn/products/visual-studio-express-vs.aspx
- Express 版有区分目标平台，请下载 “for Windows Desktop” 而不是 “for Windows”
- 你也可以使用 Visual Studio 2015 for Windows Desktop 的 Community 和 Professional 版

### CMake
- https://cmake.org/download/
- 这将用于生成 `MSVC 2015` 项目文件

## 获取源代码
- Clone 本存储库
- 在下文的说明中，`$opentoonz` 表示本存储库的根目录
- Visual Studio 无法正确识别无签名的 UTF-8 源代码。如果换行方式为 LF，且单行注释的结尾是日文的话，`MSVS` 往往会忽略了换行，把下一行也视为注释
- 为了避免该问题，请在 Git 执行下列的设置，将换行方式转换为 CRLF
  - `git config core.safecrlf true`

## 安装必须的库
因为本存储库不能存放太大的库，你需要从下列的途径安装它们。

### `lib` 和 `dll`
- `lib` 和 `dll` 文件使用了 [Git Large File Storage](https://git-lfs.github.com/) 进行管理
- 安装 `lfs` 客户端，在 `git clone` 本存储库之后，执行 `git lfs pull`

### Qt
- https://www.qt.io/download-open-source/
- Qt 是一个跨平台的 GUI 框架
- 通过 [Qt Online Installer for Windows](http://download.qt.io/official_releases/online_installers/qt-unified-windows-x86-online.exe) 或下载离线安装包安装 Qt 5.6 (64 位版)

### boost
- 需要 Boost 1.55.0 或更新的版本（已测试到 1.60.0）
- http://www.boost.org/users/history/version_1_61_0.html
- 请在上述链接中下载 boost_1_61_0.zip，然后将 boost_1_61_0 文件夹解压到 `$opentoonz/thirdparty/boost`
- 如果你打算使用 Boost 1.55.0 with Visual Studio 2013，请安装下列补丁（按照补丁文件进行修改）
  - https://svn.boost.org/trac/boost/attachment/ticket/9369/vc12_fix_has_member_function_callable_with.patch


## 生成

### 使用 CMake 生成 Visual Studio 项目
1. 启动 CMake
2. 在 `Where is the source code` 中浏览 `$opentoonz/toonz/sources`
3. 在 `Where to build the binaries` 中浏览 `$opentoonz/toonz/build`
  - 你也可以生成到其他位置
  - 如果生成目录在 git 库之中，请确定这些目录已经添加到 .gitignore
  - 如果生成目录有更改，必须将指定的目录更改到恰当的位置
4. 单击 Configure 并选择 Visual Studio 14 2015 Win64
5. 如果 Qt 没有安装到默认目录，并出现了 `Specify QT_PATH properly` 这一错误，请将 `QT_DIR` 安装目录指定到路径 `msvc2015_64`，然后重新执行 Configure
6. 单击 Generate
  - 每当 CMakeLists.txt 文件被修改，生成时会自动进行处理，无需重新执行 CMake

## 设置库
重命名下列的文件
  - `$opentoonz/thirdparty/LibJPEG/jpeg-9/jconfig.vc` → `$opentoonz/thirdparty/LibJPEG/jpeg-9/jconfig.h`
  - `$opentoonz/thirdparty/tiff-4.0.3/libtiff/tif_config.vc.h` → `$opentoonz/thirdparty/tiff-4.0.3/libtiff/tif_config.h`
  - `$opentoonz/thirdparty/tiff-4.0.3/libtiff/tiffconf.vc.h` → `$opentoonz/thirdparty/tiff-4.0.3/libtiff/tiffconf.h`
  - `$opentoonz/thirdparty/libpng-1.6.21/scripts/pnglibconf.h.prebuilt` → `$opentoonz/thirdparty/libpng-1.6.21/pnglibconf.h`
    - 注意最后一个文件的目标文件夹并不一样

在 tif_config.h 的 50 行前后添加下列的内容：
```
+#if _MSC_VER < 1900
#define snprintf _snprintf
+#endif
```

## 生成
1. 打开 `$opentoonz/toonz/build/OpenToonz.sln` 并更改为 `Release`
2. 将生成到 `$opentoonz/toonz/build/Release`

## 运行程序
### 设置程序路径
1. 将 `$opentoonz/toonz/build/Release` 里面的全部内容复制到一个恰当的文件夹

2. 打开 Qt 命令行，进入 `OpenToonz_1.0.exe` 的路径，执行命令 `windeployqt OpenToonz_1.0.exe`
  - 必须的 Qt 库文件将自动收集到 `OpenToonz_1.0.exe` 的所在文件夹
  - 其中包括下列文件：
    - `Qt5Core.dll`
    - `Qt5Gui.dll`
    - `Qt5Network.dll`
    - `Qt5OpenGL.dll`
    - `Qt5PrintSupport.dll`
    - `Qt5Script.dll`
    - `Qt5Svg.dll`
    - `Qt5Widgets.dll`
    - `Qt5Multimedia.dll`
  - 下列文件必须包含在 `OpenToonz_1.0.exe` 的所在文件夹下的相应文件夹内
    - `/bearer/qgenericbearer.dll`
    - `/bearer/qnativewifibearer.dll`
    - `/iconengines/qsvgicon.dll`
    - `/imageformats/qdds.dll`
    - `/imageformats/qgif.dll`
    - `/imageformats/qicns.dll`
    - `/imageformats/qico.dll`
    - `/imageformats/qjpeg.dll`
    - `/imageformats/qsvg.dll`
    - `/imageformats/qtga.dll`
    - `/imageformats/qtiff.dll`
    - `/imageformats/qwbmp.dll`
    - `/imageformats/qwebp.dll`
    - `/platforms/qwindows.dll`

3. 复制下载文件到 `OpenToonz_1.0.exe` 的所在文件夹
  - `$opentoonz/thirdparty/glut/3.7.6/lib/glut64.dll`
  - `$opentoonz/thirdparty/glew/glew-1.9.0/bin/64bit/glew32.dll`

4. 安装官网的 OpenToonz 安装程序，然后将安装路径的 `srv` 文件夹复制到 `OpenToonz_1.0.exe` 的所在文件夹中
  - 没有 `srv` 文件夹 OpenToonz 仍可以使用，只是不能支持 `mov` 格式的视频
  - 后文会写到如何创建 `srv` 内的文件

### 创建 stuff 文件夹
如果你之前已经安装过 OpenToonz，那这一步和后文的创建注册表项都不用处理，可以跳过这些步骤。但如果翻译有更新的话，那就需要执行这一步。

1. 将 `$opentoonz/stuff` 复制到一个恰当的文件夹

### 创建注册表项
1. 使用注册表编辑器，按照下列的路径创建注册表项并将 `$opentoonz/stuff` 文件夹的路径填写到 TOONZROOT 的字符串值
  - HKEY_LOCAL_MACHINE\SOFTWARE\OpenToonz\OpenToonz\1.0\TOONZROOT

### 运行
现在 `OpenToonz_1.0.exe` 可以运行了。

## 创建 `srv` 文件夹内的文件
OpenToonz 使用 QuickTime SDK 来支持和关联 `mov` 格式。因为 QuickTime SDK 只有 32 位版本，所以 QuickTime SDK 的 32 位文件 `t32bitsrv.exe` 将用于 64 位 和 32 位的 OpenToonz。因此，下列的步骤同样适用于 64 位 和 32 位的 OpenToonz。

### Qt
- https://www.qt.io/download-open-source/
- 通过 [Qt Online Installer for Windows](http://download.qt.io/official_releases/online_installers/qt-unified-windows-x86-online.exe) 或下载离线安装包安装安装 Qt 5.6 (32 位版)

### QuickTime SDK
1. 登录您的 Apple 开发人员 ID 并从下列链接中下载 `QuickTime 7.3 SDK for Windows.zip` 
  - https://developer.apple.com/downloads/?q=quicktime
2. 安装 QuickTime SDK 之后，复制其中的 `C:\Program Files (x86)\QuickTime SDK` 到 `$opentoonz/thirdparty/quicktime/QT73SDK`

### 使用 CMake 创建 Visual Studio 32-bit 项目
- 按照 64 位版的步骤进行操作，但有下列更改
  - `$opentoonz/toonz/build` → `$opentoonz/toonz/build32`
  - `Visual Studio 14 2015 Win64` → `Visual Studio 14 2015`
- 更改 `QT_PATH` 到 32 位版 Qt 的路径

### 生成 32 位版
1. 打开 `$opentoonz/toonz/build32/OpenToonz.sln`

### `srv` 文件夹的布局
- 复制下列的文件到 64 位版的 `srv` 文件夹中
  - 从 `$opentoonz/toonz/build32/Release`
    - t32bitsrv.exe
    - image.dll
    - tnzcore.dll
  - 从 32 位版 Qt 的安装位置
    - Qt5Core.dll
    - Qt5Network.dll
  - `$opentoonz/thirdparty/glut/3.7.6/lib/glut32.dll`

## 创建翻译文件
Qt 翻译文件首先是从源代码生成 `.ts` 文件，然后对 `.ts` 文件进行翻译，再从 `.ts` 文件生成 `.qm` 文件。如果要生成 `.ts` 和 `.qm` 文件，可以在 Visual Studio 解决方案中对名称为 `translation_` 开头的项目执行“仅用于项目”>“仅生成 translation_???”。这些文件不会在默认的“生成解决方案”中生成。
