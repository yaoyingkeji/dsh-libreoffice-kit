# 发布说明：libreoffice-kit 0.2.3 linux-x64-glibc

## 版本信息

- **版本**：0.2.3
- **平台**：linux-x64-glibc
- **引擎类型**：native（静态链接）
- **LibreOffice Core**：revision `bce0998afefdbc355585ca324285661a2170ba77`
- **发布仓库**：yaoyingkeji/dsh-libreoffice-kit（fork of deepseek-ai/dsh-libreoffice-kit）

## 新增功能

### linux-x64-glibc 原生引擎

本版本首次为 Linux x64 (glibc) 平台提供原生引擎支持：

- **可执行文件**：`bin/dsoffice`（静态链接的 LibreOffice 转换引擎）
- **程序目录**：`program/program`（LibreOffice 运行时资源）
- **构建模式**：静态链接（`DISABLE_DYNLOADING=TRUE`）
- **功能支持**：文档转换（convert）、重新计算（recalculate）
- **限制**：无 Skia GPU 渲染（`--disable-skia`），渲染图片功能部分降级

### 构建环境

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

## 发布方式

### GitHub Actions Artifact

构建成功后，产物作为 GitHub Actions artifact 上传：
- 名称：`libreoffice-kit-linux-x64-glibc`
- 保留期：30 天
- 路径：`packages/linux-x64-glibc/`

### 后续发布选项

1. **提交到仓库**：将构建产物提交到 `packages/linux-x64-glibc/`，更新 `prebuilds.json` 的 `status` 为 `built`
2. **GitHub Release**：创建版本 tag `libreoffice-kit-v0.2.3`，上传引擎 tarball
3. **npm 发布**：使用 `npm pack` + `npm publish` 发布 `@deepseek-ai/libreoffice-kit-linux-x64-glibc@0.2.3`

## 许可证

- LibreOffice：MPL-2.0
- DeepSeek Harness：MIT
- 第三方依赖：见 `licenses/LibreOffice-third-party.html`

## 验证

构建完成后，`prebuilds.json` 包含：
- `status: "built"`
- `engine.glibcMinimum`：最低 glibc 版本要求
- `files`：所有文件的 SHA256 校验和
- `source`：源码 recipe 和 core revision 信息
