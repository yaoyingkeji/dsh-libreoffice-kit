# libreoffice-kit 0.2.3 linux-x64-glibc

## 分支说明

### 概述

本分支基于上游 `deepseek-ai/dsh-libreoffice-kit`（master 分支，0.1.3 版本）创建，目标是：

1. 将仓库从 **0.1.3 升级到 0.2.3**
2. 添加 **linux-x64-glibc 原生引擎**支持（方案：上游官方编译）
3. 通过 **GitHub Actions 云端工作流**完成源码编译

### 0.2.3 来源

从上游 deepseek-ai 在 npm 发布的 `@deepseek-ai/libreoffice-kit-win32-x64@0.2.3` 平台包中提取。该包的 `sources/` 目录包含 DeepSeek 上游官方用于编译 LibreOffice 原生引擎的完整 recipe（构建脚本、patch、配置）。通过 `npm pack` 下载该包后解包提取 sources/ 目录，覆盖本地 engine/native/ 等目录完成升级。LibreOffice core 源码版本固定在 revision `bce0998afefdbc355585ca324285661a2170ba77`。

### 变更内容

- `engine/native/` 新增 12 个源文件（editor、graphite、ganesh、slideshow 相关）
- `engine/native/patches/` 新增 28 个 patch（0066-0092）
- `engine/native/patches/0093-linux-lpsolve-static-colamd.patch` — 修复 lpsolve 静态构建中 `$colamd_c` 变量未展开的问题
- `engine/wasm-source/patches/` 新增 1 个 patch
- `scripts/` 更新 10 个构建脚本
- `packages/frame-forwarding/` 新增 WebGPU 帧转发 SDK
- `packages/entry/package.json` 的 `optionalDependencies` 添加 `linux-x64-glibc`、`linux-arm64-glibc`、`koffi 3.1.1`
- `packages/linux-x64-glibc/prebuilds.json`：`executable` 改为 `bin/dsoffice`，`programDirectory` 改为 `program/program`
- `.github/workflows/build-linux-x64-native.yml` — GitHub Actions 云端编译工作流

### 构建流程

```
checkout-core.mjs → build-native.mjs (patches + autogen + make) → stage-native.mjs
```

1. `checkout-core.mjs` 从 LibreOffice core 仓库（revision `bce0998`）获取源码
2. `build-native.mjs` 应用 93 个 patch，运行 `autogen.sh` + `make build`
3. `stage-native.mjs` 将编译产物暂存到 `packages/linux-x64-glibc/`

Linux 静态构建配置：
- `--disable-dynamic-loading`（静态链接，`DISABLE_DYNLOADING=TRUE`）
- `--disable-skia`（无 GPU 渲染）
- `--disable-gui`、`--without-x`（无头模式）
- `--disable-nss`、`--with-tls=openssl`

---

## 版本信息

- **版本**：0.2.3
- **平台**：linux-x64-glibc
- **引擎类型**：native（静态链接）
- **LibreOffice Core**：revision `bce0998afefdbc355585ca324285661a2170ba77`
- **发布仓库**：yaoyingkeji/dsh-libreoffice-kit（fork of deepseek-ai/dsh-libreoffice-kit）

## 引擎详情

- **可执行文件**：`bin/dsoffice`（静态链接的 LibreOffice 转换引擎）
- **程序目录**：`program/program`（LibreOffice 运行时资源）
- **构建模式**：静态链接（`DISABLE_DYNLOADING=TRUE`）
- **功能支持**：文档转换（convert）、重新计算（recalculate）
- **限制**：无 Skia GPU 渲染（`--disable-skia`），渲染图片功能部分降级

## 构建环境

- **OS**：Ubuntu 24.04 (glibc)
- **编译器**：GCC 13
- **构建系统**：GitHub Actions（ubuntu-24.04 runner）
- **并行度**：4 核

## 构建修复

### 1. lpsolve 静态构建修复（patch 0093）

LibreOffice 上游的 `external/lpsolve/ccc.static` 脚本中，`src` 变量使用单引号赋值，导致 `$colamd_c` shell 变量不被展开，gcc 报错 `$colamd_c: linker input file not found`。

修复：将单引号改为双引号，使 `$colamd_c` 正确展开为 `../colamd/colamd.c`。

### 2. 构建依赖补全

GitHub Actions runner 上需安装以下额外依赖：

| 依赖 | 用途 |
|------|------|
| `libfontconfig-dev` | BUILD side configure 检查 fontconfig |
| `libfreetype-dev` | 字体渲染 |
| `libharfbuzz-dev` | 文本 shaping |
| `libgraphite2-dev` | Graphite 字体引擎 |
| `gperf` | 完美哈希函数生成器 |
| `libxml2-dev` / `libxslt1-dev` | XML 处理 |
| `libjpeg-dev` / `libpng-dev` / `libtiff-dev` | 图像格式支持 |
| `libglib2.0-dev` | GLib 工具库 |

## 包结构

```
packages/linux-x64-glibc/
├── package.json          # @deepseek-ai/libreoffice-kit-linux-x64-glibc@0.2.3
├── prebuilds.json        # 引擎清单（status: built）
├── bin/
│   └── dsoffice          # 静态链接的可执行文件
├── program/              # LibreOffice 运行时资源
│   └── program/          # 库文件、配置、资源
├── sources/              # 对应源码（构建 recipe + patches + core-changes）
└── licenses/             # 许可证文件
    ├── LibreOffice-MPL-2.0.txt
    ├── DeepSeek-Harness-MIT.txt
    └── LibreOffice-third-party.html
```

## 许可证

- LibreOffice：MPL-2.0
- DeepSeek Harness：MIT
- 第三方依赖：见 `licenses/LibreOffice-third-party.html`
