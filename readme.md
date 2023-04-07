## DwgFinder

### 基本使用
DwgFinder的客户端是一个网页，在浏览器中打开就可以使用。

DwgFinder的搜索语句与在JIRA中搜索问题时使用的JQL类似，例如下面的语句用来查找所有块参照数量 > 50000的图纸，并按照块参照数量降序排列：
```
AcDbBlockReference > 50000 order by AcDbBlockReference DESC
```

通过搜索语句找到图纸之后，可以点击图纸的Path列下载单张图纸，也可以点击`Download All`下载所有满足条件的图纸。(如果搜索到的图纸较多，点击`Download All`之后可能需要等待较长的时间才能开始下载，这种情况下建议先进一步筛选搜索结果，使得图纸数量在100以内，然后再点击`Download All`下载。)

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
AcDbText + AcDbMText > 10000
```

运算符主要包含下面几种：

| 运算符 | 含义 |
| ----- | ---- |
| = | 相等 |
| <> or != | 不等于 |
| < | 小于 |
| > | 大于 |
| <= | 小于等于 |
| >= | 大于等于 |
| AND | 逻辑运算且 |
| OR | 逻辑运算或 |

使用逻辑运算符可以扩大或缩小查找范围，例如:
```
AcDbText > 10000 or AcDbMText > 10000
AcDbDimension > 10000 and AcDbLeader > 10000
```

`orderby_clause`的形式如下：
```
ORDER BY field1 ASC, field2 DESC
```

可以使用多个字段进行排序，两个字段中间用`,`隔开，每个字段可指定是升序(`ASC`)还是降序(`DESC`)排列。

### 字段说明
#### 1. 实体数量
该类型的字段统计了模型空间和所有布局空间中各种实体的个数。字段以实体的类型名命名，例如AcDbText表示单行文字，AcDbBlockReference表示块参照。自定义实体的字段名为`CustomEntity`。由于实体类型之间存在着继承关系，子类型的个数也会计入到父类型的个数中。这个机制有时会比较自然，比如AcDbUnderlayReference中记录了AcDbDgnReference、AcDbDwfReference、AcDbPdfReference的总数，并且AcDbEntity的个数就是依赖这个机制计算出来的；有时又会比较奇怪，比如AcDbTable的个数也汇总到了AcDbBlockReference中，这时可以通过减法运算从AcDbBlockReference中排除AcDbTable：`AcDbBlockReference - AcDbTable > 1000`。

| 字段名 | 含义 |
| ----- | ---- |
| AcDbEntity | 所有实体个数 |
| AcDbBlockReference | 块参照个数 |
| AcDbMInsertBlock | 多重插入块个数 |
| AcDbTable | 表格个数 |
| AcDbViewRepBlockReference | 和视口关联的块参照个数 |
| AcDbCamera | 相机个数 |
| AcDbFace | 三维面个数 |
| AcDbFcf | 公差个数 |
| AcDbFrame | OLE对象总数 |
| AcDbOleFrame | OLE1对象个数 |
| AcDbOle2Frame | OLE2对象个数 |
| AcDbHatch | 图案填充个数 |
| AcDbImage | 图像个数 |
| AcDbRasterImage | 光栅图像个数 |
| AcDbWipeout | 区域覆盖个数 |
| AcDbLight | 光源个数 |
| AcDbMLeader | 多重引线个数 |
| AcDbMline | 多线个数 |
| AcDbMPolygon | MPolygon个数 |
| AcDbMText | 多行文字个数 |
| AcDbPoint | 点个数 |
| AcDbPolyFaceMesh | 多面网格个数 |
| AcDbPolygonMesh | 多边形网格个数 |
| AcDbProxyEntity | 代理实体个数（均为0，建议使用CustomEntity） |
| AcDbSection | Section个数 |
| AcDbShape | 形个数 |
| AcDbSolid | 实体个数 |
| AcDbText | 单行文字个数 |
| AcDbAttribute | 属性个数 |
| AcDbAttributeDefinition | 属性定义个数 |
| AcDbTrace | 跟踪个数 |
| AcDbViewport | 视口个数 |
| AcDbCurve | 曲线个数，包含多段线、圆弧、圆、椭圆、射线、样条曲线、构造线等 |
| AcDb2dPolyline | 二维多段线个数 |
| AcDb3dPolyline | 三维多段线个数 |
| AcDbArc | 圆弧个数 |
| AcDbCircle | 圆个数 |
| AcDbEllipse | 椭圆个数 |
| AcDbLeader | 引线个数 |
| AcDbLine | 线段个数 |
| AcDbPolyline | 多段线个数 |
| AcDbRay | 射线个数 |
| AcDbSpline | 样条曲线个数 |
| AcDbHelix | 螺旋线个数 |
| AcDbXline | 构造线个数 |
| AcDbDimension | 标注个数 |
| AcDb2LineAngularDimension | 通过两条线段定义的角度标注个数 |
| AcDb3PointAngularDimension | 通过三点定义的角度标注个数 |
| AcDbAlignedDimension | 对齐标注个数 |
| AcDbArcDimension | 弧长标注个数 |
| AcDbDiametricDimension | 直径标注个数 |
| AcDbOrdinateDimension | 坐标标注个数 |
| AcDbRadialDimension | 半径标注个数 |
| AcDbRadialDimensionLarge | 折引半径标注个数 |
| AcDbRotatedDimension | 转角标注个数 |
| AcDb3dSolid | 三维实体个数 |
| AcDbBody | Body个数 |
| AcDbRegion | 面域个数 |
| AcDbSubDMesh | 网格个数 |
| AcDbSurface | 曲面个数，包含拉伸曲面、放样曲面、扫掠曲面等 |
| AcDbExtrudedSurface | 拉伸曲面个数 |
| AcDbLoftedSurface | 放样曲面个数 |
| AcDbNurbSurface | NURBS曲面个数 |
| AcDbPlaneSurface | 平面个数 |
| AcDbRevolvedSurface | 旋转曲面个数 |
| AcDbSweptSurface | 扫掠曲面个数 |
| AcDbViewBorder | 视口边界个数 |
| AcDbViewSymbol | ViewSymbol个数 |
| AcDbDetailSymbol | DetailSymbol个数 |
| AcDbSectionSymbol | SectionSymbol个数 |
| AcDbUnderlayReference | 参考底图个数 |
| AcDbDgnReference | DGN底图个数 |
| AcDbDwfReference | DWF底图个数 |
| AcDbPdfReference | PDF底图个数 |
| AcDbArcAlignedText | 弧线对齐文本个数 |
| CustomEntity | 自定义实体个数 |

#### 2. 符号表记录数量
该类型的字段记录了九个符号表中记录的个数。

| 字段名 | 含义 |
| ----- | ---- |
| BlockTableRecord | 块表记录个数 |
| LayerTableRecord | 图层表记录个数 |
| TextStyleTableRecord | 文字样式表记录个数 |
| LinetypeTableRecord | 线型表记录个数 |
| ViewTableRecord | 视图表记录个数 |
| UCSTableRecord | UCS表记录个数 |
| ViewportTableRecord | 视口表记录个数 |
| RegAppTableRecord | RegApp表记录个数 |
| DimStyleTableRecord | 标注样式表记录个数 |

#### 3. 命令对象词典记录数量
该类型的字段记录了命令对象词典中常见记录的个数。

| 字段名 | 含义 |
| ----- | ---- |
| Color | 命令对象词典中Color的记录个数 |
| DataLink | 命令对象词典中DataLink的记录个数 |
| DetailViewStyle | 命令对象词典中DetailViewStyle的记录个数 |
| Groups | 组的个数 |
| Layout | 布局个数 |
| Material | 材质个数 |
| MLeaderStyle | 多重引线样式个数 |
| MLineStyle | 多线样式个数 |
| PlotSettings | 打印设置个数 |
| PlotStyleName | 命令对象词典中PlotStyleName的记录个数 |
| ScaleList | 比例列表中的记录个数 |
| SectionViewStyle | 命令对象词典中SectionViewStyle的记录个数 |
| TableStyle | 表格样式个数 |
| VisualStyle | 视觉样式个数 |

#### 4. 动态块
该类型的字段记录了动态块定义及动态块参照的个数。（由于动态块的部分接口未实现，动态块参照的个数一直为0。）

| 字段名 | 含义 |
| ----- | ---- |
| DynBlockDefinition | 动态块定义个数 |
| DynBlockReference | 动态块参照个数 |

#### 5. 关联阵列
记录关联阵列的个数。（同时也统计了参数化对象和工程视图的个数，但是由于部分接口未实现，这两个字段的值一直为0。）

| 字段名 | 含义 |
| ----- | ---- |
| AssocArray | 关联阵列个数 |
| ConstraintGroup | 参数化对象个数 |
| DrawingView | 工程视图个数 |

#### 6. 模型空间块参照的最大嵌套层次
记录模型空间块参照的最大嵌套层次，布局空间中块参照的未统计。

| 字段名 | 含义 |
| ----- | ---- |
| MaxNestedBlockLevel | 模型空间块参照的最大嵌套层次 |

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
    "db_path": "D:\\test.db",
    "db_table": "EntityCount",
    "db_columns": [
        "AcDbEntity",
        "AcDbBlockReference",
        ...
    ],
    "entry_per_page": 50,
    "watched_path": "D:\\testFiles",
    "worker_time_interval": 30
}
```
`db_path`：SQLite数据库文件的位置；  
`db_table`：数据库表名；  
`db_columns`: 数据库中的字段名；  
`entry_per_page`: 搜索页面中每页最多显示的图纸数量；  
`watched_path`：监控的包含图纸文件的文件夹路径；  
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

### 如何添加新的字段
1. 实现DbReader的具体类

`DbReader`是一个抽象类，需实现其中的`read`和`getResult`接口，可参考`DumbReader`的实现。

```c++
class DbReader
{
public:
	virtual ~DbReader() = default;
	virtual std::tuple<bool, std::wstring> read(AcDbDatabase* pDb) = 0;
	virtual const std::map<std::wstring, unsigned int>& getResult() = 0;
};
```

2. 在`main.cpp`中注册DbReader具体类的对象

```c++

	DbReaderManager dbrMgr;
	dbrMgr.registerReader(&countEntity);
	dbrMgr.registerReader(&symbolTableReader);
	dbrMgr.registerReader(&namedObjectDictReader);
	dbrMgr.registerReader(&dynBlockReader);
	dbrMgr.registerReader(&assocObjectReader);
	dbrMgr.registerReader(&nestedBlockLevelReader);
```

3. 编译DwgRead，并使用生成的exe替换`DwgFinder/bin/`目录下的DwgRead.exe

4. 在`config.json`中添加新的字段

将新添加的字段补充到`db_columns`中。

```json
    "db_columns": [
        "AcDbEntity",
        "AcDbBlockReference",
        ...
    ],
```

5. 重新生成数据库

如果已经有个Web服务器正在运行，可以考虑将`config.json`中的`db_path`修改为另一个文件，然后再依次执行以下命令生成新的数据库：

```shell
python Scripts/initdb.py
python -m DwgFinder.main
```
生成好之后将正在运行的Web服务器关闭并重新启动：

```shell
waitress-serve "WebServer:app"
```
