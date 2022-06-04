# -*- coding=utf-8 -*-
# @Time :2022/6/4 16:38
# @Author :Run Luo
# @Site : 
# @File : geoip.py
# @Software : PyCharm

import geoip2.database
from pyecharts.charts import Geo
# 导入配置项
from pyecharts import options as opts
# ChartType：图标类型，SymbolType：标记点类型
from pyecharts.globals import ChartType, SymbolType

reader = geoip2.database.Reader('GeoLite2-City.mmdb')
localhost='10.17.39.220'
local_name='Wuhan'
local_location=[114.2662, 30.5851]
all_citys={}
all_citys['Wuhan']=local_location
lines=[]
with open('result.txt','r') as f:
    data=f.readlines()
    for item in data:
        try:
            src,des=item.strip().split(',')
            des_name='Wuhan'
            src_name='Wuhan'
            if src!=localhost:
                src_response=reader.city(src)
                if 'en' in src_response.city.names.keys():
                    all_citys[src_response.city.names['en']]=[src_response.location.longitude,src_response.location.latitude]
                    src_name=src_response.city.names['en']
            if des!=localhost:
                des_response = reader.city(des)
                if 'en' in des_response.city.names.keys():
                    all_citys[des_response.city.names['en']]=[des_response.location.longitude,des_response.location.latitude]
                    des_name=des_response.city.names['en']
            if des_name!=src_name:
                lines.append([src_name,des_name])
        except RuntimeError as e:
            pass

geo = Geo()
# 新增坐标点，添加名称跟经纬度, 也就是当地图中不包含这个地名的坐标时，自己加入！
for key,value in all_citys.items():
    geo.add_coordinate(key, value[0], value[1])

# 地图类型，世界地图可换为world
geo.add_schema(maptype="world")
# 添加数据点
# geo.add("", [("Beijing", 10), ("Shanghai", 20), ("Guangzhou", 30), ("Chengdu", 40), ("Harbin", 50), ("new", 40)], type_=ChartType.EFFECT_SCATTER)
# 添加流向，type_设置为LINES，涟漪配置为箭头，提供的标记类型包括 'circle', 'rect', 'roundRect', 'triangle',
# 'diamond', 'pin', 'arrow', 'none'
geo.add("geo-lines",
        lines[:1000],
        type_=ChartType.LINES,
        effect_opts=opts.EffectOpts(symbol=SymbolType.ARROW, symbol_size=2, color="yellow"),
        linestyle_opts=opts.LineStyleOpts(curve=0.2),
        is_large=True)
# 不显示标签
geo.set_series_opts(label_opts=opts.LabelOpts(is_show=True))
# 设置图标标题，visualmap_opts=opts.VisualMapOpts()为左下角的视觉映射配置项
geo.set_global_opts(visualmap_opts=opts.VisualMapOpts(), title_opts=opts.TitleOpts(title="流量世界流向图"))
# 直接在notebook里显示图表
geo.render_notebook()
# 生成html文件，可传入位置参数
geo.render("流量地图.html")