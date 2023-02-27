## DwgFinder

### 基本使用
DwgFinder的客户端是一个网页，在浏览器中打开就可以使用。

DwgFinder的搜索语句与在JIRA中搜索问题时使用的JQL类似，例如下面的语句用来查找所有块参照数量 > 50000的图纸，并按照块参照数量降序排列：
```
AcDbBlockReference > 50000 order by AcDbBlockReference DESC
```

通过搜索语句找到图纸之后，可以点击图纸的Path列下载单个图纸，也可以点击`Download All`下载所有图纸。

### 语法说明
DwgFinder的搜索语句包含如下两个部分：
```
search_condition [orderby_clause]
```
其中，`search_condition`必须要有，`orderby_clause`可有可无。

`search_condition`形式如下：
```
left_expression OPERATOR right_expression
```

例如，可以使用如下语句进行搜索:
```
AcDbEntity > 1000000
AcDbText + AcDbMText > 10000;
```

运算符包含主要下面几种：
| 运算符 | 含义 |
|-------|------|
| = | 相等 |
| <> or != | 不等于 |
| < | 小于 |
| > | 大于 |
| <= | 小于等于 |
| >= | 大于等于 |
| AND | 逻辑运算且 |
| OR | 逻辑运算或 |

使用逻辑运算符可以缩小或扩大查找范围，例如:
```
AcDbText > 10000 or AcDbMText > 10000
AcDbDimension > 10000 and AcDbLeader > 10000
```

`orderby_clause`的形式如下：
```
ORDER BY field1 ASC, field2 DESC
```

可以使用多个字段进行排序，两个字段中间用`,`隔开，每个字段可指定是升序(`ASC`)还是降序(`DESC`)排列。

### 后端服务启动方式
#### 安装Python及依赖
Python下载链接：https://www.python.org/ftp/python/3.8.10/python-3.8.10-amd64.exe  
项目代码使用poetry管理其依赖，参考其文档进行安装：[poetry安装文档](https://python-poetry.org/docs/#installation)；  
安装好poetry之后，在项目代码根目录下执行命令安装依赖：
```
poetry install --no-root
```

#### 修改配置文件
配置文件为项目代码根目录下的config.json，示例配置文件中包含如下配置项：
```json
{
    "db_path": "D:/test.db",
    "db_table": "EntityCount",
    "watched_path": "D:\\testFiles",
    "zwcad_exe": "D:/Program Files/ZWSOFT/ZWCAD 2023/ZWCAD.exe",
    "zwcad_args": [
        "/company",
        "ZWSoft",
        "/product",
        "zwcad",
        "/language",
        "en-US"
    ],
    "zwcad_window_title": ".*ZWCAD 2023 Pro.*",
    "worker_time_interval": 30
}
```
`db_path`：SQLite数据库文件的位置；  
`db_table`：数据库表名；  
`watched_path`：监控的包含图纸文件的文件夹路径；  
`zwcad_exe`：ZWCAD可执行文件路径（必须为英文版）；  
`zwcad_args`：ZWCAD的启动参数；  
`zwcad_window_title`: 与ZWCAD主窗口title匹配的正则表达式，一般只需改动年度即可；  
`worker_time_interval`: 进入待命阶段之后工作线程唤醒的时间间隔，单位为秒；  

#### 创建SQLite数据库
仅在第一次启动前或者数据库结构有改动，需要更新的时候才运行：
```
# 在项目代码的根目录下运行
python Scripts/initdb.py
```

#### 启动Web服务器
在项目代码根目录下执行命令：
```
waitress-serve "WebServer:app"
```

#### 启动监视并扫描dwg文件的工具
在项目代码根目录下执行命令：
```
python -m DwgFinder.main
```
