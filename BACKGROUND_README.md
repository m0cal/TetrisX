# 背景图片说明

## 如何添加背景图片

1. 准备一张PNG格式的背景图片，命名为 `background.png`
2. 将图片放置在以下路径：`Source/Resources/Images/background.png`
3. 重新编译并运行游戏

## 背景图片要求

- 格式：PNG
- 名称：background.png
- 位置：Source/Resources/Images/background.png
- 大小：任意尺寸（程序会自动缩放适配窗口大小）

## 编译方法

```bash
cd /home/mocal/myCode/TetrisX
make -C build
```

## 运行游戏

```bash
cd /home/mocal/myCode/TetrisX/build
./tetris
```

## 注意事项

- 如果没有背景图片文件，游戏仍然可以正常运行，只是没有背景
- 背景图片会在所有游戏元素下方渲染
- 图片会自动缩放以适配整个窗口大小
