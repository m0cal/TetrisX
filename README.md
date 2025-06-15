# TetrisX

经典俄罗斯方块游戏，已重构并迁移到 SFML 3。

## 特性

- 经典的俄罗斯方块游戏玩法
- 现代化的 SFML 3 图形渲染
- 流畅的游戏体验
- 线条清除效果
- 幽灵方块预览
- 速度递增系统

## 构建要求

- CMake 3.16+
- C++20 编译器
- SFML 3.x

## 构建说明

```bash
# 克隆仓库
git clone <repository-url>
cd TetrisX

# 创建构建目录
mkdir build && cd build

# 配置和构建
cmake ..
make

# 运行游戏
./tetris
```

## 游戏控制

- **C/Z**: 旋转方块
- **方向键**: 移动方块
- **下箭头**: 软降
- **空格**: 硬降
- **Enter**: 重新开始

## 迁移说明

本项目已从 SFML 2.5 成功迁移到 SFML 3，主要变更包括：

- API 更新：键盘输入、窗口创建、位置设置等
- 构建系统现代化
- 错误处理改进
- 代码质量提升

详细的迁移记录请查看 [REFACTORING_NOTES.md](REFACTORING_NOTES.md)。

## 测试

运行迁移测试脚本：

```bash
./test_migration.sh
```

## 原始视频

原始项目的开发视频：https://youtu.be/vkS1fY_UTyg
