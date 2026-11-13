# 分支说明：libreoffice-kit-0.2.3-linux-x64

## 概述

本分支基于上游 `deepseek-ai/dsh-libreoffice-kit`（master 分支，0.1.3 版本）创建，目标是：

1. 将仓库从 **0.1.3 升级到 0.2.3**
2. 添加 **linux-x64-glibc 原生引擎**支持（方案 A：上游官方编译）
3. 通过 **GitHub Actions 云端工作流**完成源码编译

## 变更内容

### 1. 构建脚本升级到 0.2.3

从 npm `@deepseek-ai/libreoffice-kit-win32-x64@0.2.3` 平台包的 `sources/` 目录提取 0.2.3 版本构建脚本：

- `engine/native/` 新增 12 个源文件（editor、graphite、ganesh、slideshow 相关）
- `engine/native/patches/` 新增 28 个 patch（0066-0092）
- `engine/native/patches/0093-linux-lpsolve-static-colamd.patch` — 修复 lpsolve 静态构建中 `$colamd_c` 变量未展开的问题
- `engine/wasm-source/patches/` 新增 1 个 patch
- `scripts/` 更新 10 个构建脚本
- `packages/frame-forwarding/` 新增 WebGPU 帧转发 SDK

### 2. 版本号升级

| 包 | 旧版本 | 新版本 |
|---|--------|--------|
| 根 workspace | 0.1.3 | 0.2.3 |
| entry（@deepseek-ai/libreoffice-kit） | 0.1.3 | 0.2.3 |
| win32-x64 / win32-arm64 | 0.1.3 | 0.2.3 |
| darwin-x64 / darwin-arm64 | 0.1.3 | 0.2.3 |
| wasm | 0.1.3 | 0.2.3 |
| linux-x64-glibc | 0.1.1 | 0.2.3 |
| linux-arm64-glibc | 0.1.1 | 0.2.3 |

### 3. linux-x64-glibc 原生引擎支持

- `packages/entry/package.json` 的 `optionalDependencies` 添加 `linux-x64-glibc`、`linux-arm64-glibc`、`koffi 3.1.1`
- `packages/linux-x64-glibc/prebuilds.json`：
  - `executable` 从 `bin/libreoffice-kit` 改为 `bin/dsoffice`
  - `programDirectory` 从 `program` 改为 `program/program`
  - `status` 为 `unbuilt`（待 CI 编译完成后更新为 `built`）

### 4. GitHub Actions 工作流

- `.github/workflows/build-linux-x64-native.yml` — 在 ubuntu-24.04 runner 上执行上游官方编译流程

## 构建流程

```
checkout-core.mjs → build-native.mjs (patches + autogen + make) → stage-native.mjs
```

1. `checkout-core.mjs` 从 LibreOffice core 仓库（revision `bce0998`）获取源码
2. `build-native.mjs` 应用 93 个 patch，运行 `autogen.sh` + `make build`
3. `stage-native.mjs` 将编译产物暂存到 `packages/linux-x64-glibc/`

### Linux 静态构建配置

- `--disable-dynamic-loading`（静态链接，`DISABLE_DYNLOADING=TRUE`）
- `--disable-skia`（无 GPU 渲染）
- `--disable-gui`、`--without-x`（无头模式）
- `--disable-nss`、`--with-tls=openssl`

## 提交记录

| 提交 | 说明 |
|------|------|
| `be28dab` | 升级到 libreoffice-kit 0.2.3 并添加 linux-x64-glibc 原生引擎支持 |
| `eb620cd` | 添加 GitHub Actions 工作流：Linux x64 原生引擎云端编译 |
| `acf5d9a` | 修复构建：添加 gperf 等完整构建依赖 |
| (待提交) | 修复 lpsolve 静态构建 + 分支说明 + 发布说明 |

## 身份

所有提交使用 yaoyingkeji 身份：
- 姓名：深圳市耀影科技有限公司
- 邮箱：yaoyingkeji@yaoying.vip
