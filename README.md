# BLIVE_M3U8
BiliBili 直播取流工具

## 用法
``blive_m3u8 <PATH_CONFIG>``

传入配置文件地址

## 配置编写
``````json
{
    "cid": "xxx",
    "mid": "xxx",
    "qn": "xxx",
    "out_path": "xxx"
}
``````
``cid``: 直播间长ID
``mid``: 主播ID
``qn``: 目标画质
``out_path``: 输出目录

| qn    | 画质      |
| ----- | --------- |
| 250   | 720P超清  |
| 400   | 1080P蓝光 |
| 10000 | 1080P原画 |


> [!NOTE]
> ``out_path``必须提前创建

## 构建
项目使用 [Meson](https://mesonbuild.com/) 构建，依赖 [yyjson](https://github.com/ibireme/yyjson)，[libcurl](https://curl.se/libcurl/) 和 ``ffmpeg`` 命令

``````bash
meson setup build
cd build
meson compile
``````
