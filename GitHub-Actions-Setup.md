# GitHub Actions 设置指南

本文档说明如何为 DuckChat 项目配置和使用 GitHub Actions。

## 📁 已创建的工作流

### 1. **构建和测试** (`.github/workflows/build-and-test.yml`)
- **触发条件**: 推送到 main/develop 分支，或针对 main 分支的 PR
- **功能**: 
  - 多平台构建测试 (Ubuntu latest, Ubuntu 20.04)
  - 多编译器支持 (g++-9, g++-11, clang++-12)
  - 自动安装依赖
  - 验证二进制文件生成

### 2. **代码质量检查** (`.github/workflows/code-quality.yml`)
- **触发条件**: 推送到 main/develop 分支，或针对 main 分支的 PR
- **功能**:
  - 代码格式检查 (clang-format)
  - 静态代码分析 (cppcheck)
  - 额外代码检查 (clang-tidy)

### 3. **内存检查和测试** (`.github/workflows/memory-check.yml`)
- **触发条件**: 推送到 main/develop 分支，或针对 main 分支的 PR
- **功能**:
  - 内存泄漏检测 (Valgrind)
  - 地址消毒器 (AddressSanitizer)
  - 未定义行为消毒器 (UndefinedBehaviorSanitizer)
  - 基本功能测试

### 4. **文档生成** (`.github/workflows/docs.yml`)
- **触发条件**: 推送到 main 分支，且文档或头文件有变更
- **功能**:
  - 自动生成 Doxygen 文档
  - 部署到 GitHub Pages
  - 支持中文文档

### 5. **性能基准测试** (`.github/workflows/benchmark.yml`)
- **触发条件**: 推送到 main 分支，或针对 main 分支的 PR
- **功能**:
  - 编译时间对比
  - 启动时间测试
  - 内存使用分析
  - 二进制大小对比

## 🔧 配置文件说明

### `.clang-format`
- 基于 Google 风格的代码格式化配置
- 适配 C++17 标准
- 统一项目的代码风格

### `.github/PULL_REQUEST_TEMPLATE.md`
- 标准化的 PR 模板
- 包含完整的检查清单
- 针对 DuckChat 项目的特殊检查项

## 🚀 使用方法

### 1. 自动触发
工作流会在以下情况下自动运行：
- 代码推送到指定分支
- 创建或更新 Pull Request
- 文档文件变更（仅文档工作流）

### 2. 手动触发
可以在 GitHub Actions 页面手动运行工作流：
1. 进入项目的 Actions 页面
2. 选择相应的工作流
3. 点击 "Run workflow"

### 3. 查看结果
- 工作流运行状态会在 PR 中显示
- 详细的日志可以在 Actions 页面查看
- 文档生成后会自动部署到 GitHub Pages

## 🔑 GitHub Token 配置

### 关于 GitHub Key
**好消息**: 大部分工作流不需要手动配置 GitHub Key！

#### 自动提供的权限
GitHub 会自动为每个工作流运行提供 `GITHUB_TOKEN`，包含以下权限：
- 读取仓库内容
- 创建和更新检查状态
- 写入 Issues 和 PR（如果需要）
- 部署到 GitHub Pages（需要额外配置）

#### 需要额外配置的情况
只有以下情况需要手动配置 Secrets：

1. **GitHub Pages 部署**
   - 在仓库设置中启用 GitHub Pages
   - 在 Settings > Pages > Source 选择 "GitHub Actions"

2. **如果需要推送代码**
   - 在 Settings > Secrets and variables > Actions 中添加 `PAT_TOKEN`
   - 权限需要包含 `repo:write`

3. **如果需要访问外部服务**
   - 在 Settings > Secrets and variables > Actions 中添加相应的 Secrets

### 配置 GitHub Pages（可选）
如果需要自动部署文档：

1. 进入仓库的 Settings 页面
2. 滚动到 "Pages" 部分
3. 在 "Source" 中选择 "GitHub Actions"
4. 保存设置

## 🛠️ 本地开发建议

### 1. 安装相同的工具
```bash
# Ubuntu/Debian
sudo apt-get install clang-format cppcheck clang-tidy valgrind doxygen graphviz

# macOS
brew install clang-format cppcheck clang-tidy valgrind doxygen graphviz
```

### 2. 本地代码格式化
```bash
# 格式化所有代码
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

# 检查格式
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format --dry-run --Werror
```

### 3. 本地静态分析
```bash
# cppcheck
cppcheck --enable=all --std=c++17 server/ client/ common/

# clang-tidy
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-tidy -std=c++17
```

### 4. 本地内存检查
```bash
# 编译调试版本
./build.sh clean
./build.sh all

# 使用 Valgrind 检查
valgrind --leak-check=full ./server/server
```

## 📊 工作流状态徽章

可以在 README.md 中添加状态徽章：

```markdown
### CI/CD 状态

[![Build and Test](https://github.com/laipuran/DuckChat/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/laipuran/DuckChat/actions/workflows/build-and-test.yml)
[![Code Quality](https://github.com/laipuran/DuckChat/actions/workflows/code-quality.yml/badge.svg)](https://github.com/laipuran/DuckChat/actions/workflows/code-quality.yml)
[![Memory Check](https://github.com/laipuran/DuckChat/actions/workflows/memory-check.yml/badge.svg)](https://github.com/laipuran/DuckChat/actions/workflows/memory-check.yml)
[![Documentation](https://github.com/laipuran/DuckChat/actions/workflows/docs.yml/badge.svg)](https://github.com/laipuran/DuckChat/actions/workflows/docs.yml)
[![Benchmark](https://github.com/laipuran/DuckChat/actions/workflows/benchmark.yml/badge.svg)](https://github.com/laipuran/DuckChat/actions/workflows/benchmark.yml)
```

## 🐛 常见问题

### Q: 工作流运行失败怎么办？
A: 
1. 查看 Actions 页面的详细日志
2. 检查代码是否符合格式要求
3. 确认依赖是否正确安装
4. 验证代码是否能正常编译

### Q: 如何跳过某个检查？
A: 
- 在 PR 描述中添加 `[skip ci]` 可以跳过所有 CI
- 修改工作流文件中的条件来调整触发规则

### Q: 如何添加新的检查？
A: 
1. 在 `.github/workflows/` 目录下创建新的 yml 文件
2. 参考现有工作流的格式
3. 提交后会自动生效

### Q: GitHub Pages 部署失败？
A: 
1. 确认仓库设置中已启用 GitHub Pages
2. 检查工作流权限设置
3. 查看 Actions 日志中的具体错误信息

## 📝 维护建议

1. **定期更新**: 定期检查和更新 Actions 版本
2. **监控性能**: 关注工作流运行时间，优化过慢的步骤
3. **日志分析**: 定期分析失败日志，改进代码质量
4. **文档更新**: 及时更新本文档，反映最新的配置变化

---

如有其他问题，请查看 [GitHub Actions 官方文档](https://docs.github.com/en/actions) 或提交 Issue。