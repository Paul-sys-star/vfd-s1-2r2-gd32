# 图纸目录

本目录存放由 `hardware/tools/make_figures.py` 生成的 4 张工程图（PNG，未随仓库上传）：

| 文件 | 内容 |
|---|---|
| `01_主电路接线图.png` | 主电路通路、采样、辅助电源、上下三层连接关系 |
| `02_控制框图.png` | 给定→斜坡→V/F→SVPWM→TIMER0→IPM，含保护反馈 |
| `03_端子定义图.png` | 主回路与与制端子功能、载流、接线注意事项 |
| `04_VF曲线与降额.png` | V/F 曲线、载波降额、环境温度降额 |

重新生成（需要 matplotlib + Microsoft YaHei 字体）：

```bash
cd hardware/tools
pip install matplotlib
python make_figures.py
```

脚本内的坐标与文字可直接修改，适合按自己的机柜尺寸重排版。
