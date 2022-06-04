# -*- coding=utf-8 -*-
# @Time :2022/6/4 20:24
# @Author :Run Luo
# @Site : 
# @File : gif.py
# @Software : PyCharm


# !/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2020/2/23 13:13
# @Author  : ystraw
# @Site    :
# @File    : t3.py
# @Software: PyCharm Community Edition
# @function: 绘制城市动态流向图

# 导入Geo包，注意1.x版本的导入跟0.x版本的导入差别
# 更新方法：pip install  --upgrade pyecharts
from pyecharts.charts import Geo
# 导入配置项
from pyecharts import options as opts
# ChartType：图标类型，SymbolType：标记点类型
from pyecharts.globals import ChartType, SymbolType

geo = Geo()
# 新增坐标点，添加名称跟经纬度, 也就是当地图中不包含这个地名的坐标时，自己加入！
geo.add_coordinate('new', 122.480539, 35.235929)

# 地图类型，世界地图可换为world
geo.add_schema(maptype="world")
# 添加数据点
geo.add("", [("Beijing", 10), ("Shanghai", 20), ("Guangzhou", 30), ("Chengdu", 40), ("Harbin", 50), ("new", 40)], type_=ChartType.EFFECT_SCATTER)
# 添加流向，type_设置为LINES，涟漪配置为箭头，提供的标记类型包括 'circle', 'rect', 'roundRect', 'triangle',
# 'diamond', 'pin', 'arrow', 'none'
geo.add("geo-lines",
        [('北京', '广州'), ('上海', '上海'), ('广州', '成都'), ('成都', '哈尔滨'), ('新地点', '哈尔滨')],
        type_=ChartType.LINES,
        effect_opts=opts.EffectOpts(symbol=SymbolType.ARROW, symbol_size=10, color="yellow"),
        linestyle_opts=opts.LineStyleOpts(curve=0.2),
        is_large=True)
# 不显示标签
geo.set_series_opts(label_opts=opts.LabelOpts(is_show=True))
# 设置图标标题，visualmap_opts=opts.VisualMapOpts()为左下角的视觉映射配置项
geo.set_global_opts(visualmap_opts=opts.VisualMapOpts(), title_opts=opts.TitleOpts(title="城市动态流向图"))
# 直接在notebook里显示图表
geo.render_notebook()
# 生成html文件，可传入位置参数
geo.render("城市动态流向图2.html")