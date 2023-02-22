## DwgFinder

### 基本使用
DwgFinder的客户端是一个网页，在浏览器中打开就可以使用。

DwgFinder的搜索语句与在JIRA中搜索问题时使用的JQL类似，例如下面的语句用来查找所有块参照数量 > 50000的图纸，并按照块参照数量降序排列：
```
AcDbBlockRerence > 50000 order by AcDbBlockReference DESC
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
