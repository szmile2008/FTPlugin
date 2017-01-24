[TOC]

#接口概要

开放接口基于PC客户端获取数据，提供给用户使用

开放接口分为**低频接口**、**订阅接口**和**高频接口**：



**低频接口**主要用来获取股票市场静态和全局的数据，让用户得到股票的基本信息，不允许高频调用。

如果要实时获取数据，则需要调用高频接口。



**订阅接口**是用来管理高频接口使用额度，包括订阅、退订和查询额度。

*订阅*：在使用高频接口前，需要订阅想要的数据。订阅成功后，则可以使用高频接口获取；订阅各类数据有额度限制：

| 订阅数据 | 订阅额度 |
| ---- | ---- |
| K线   | 150  |
| 逐笔   | 5    |
| 报价   | 50   |
| 摆盘   | 50   |

*查询额度*：用来查询现在各项额度占用情况。用户可以看到每一种类型数据都有订阅了哪些股票；然后利用退订操作来去掉不需要的股票数据。

*退订*：用户可以退订指定股票和指定数据类型，空出额度。

**如果数据不订阅，直接调用高频接口则会返回失败。**



**高频接口**可以获取实时数据，可以针对小范围内的股票频繁调用；比如需要跟踪某个股票的逐笔和摆盘变化等；在调用之前需要将频繁获取的数据订阅注册。





#接口列表


##低频接口
```python
get_trading_days(market, start_date=None, end_date=None)
get_stock_basicinfo(market, stock_type='STOCK')
get_history_kline(code, start='', end='', ktype='K_DAY', autype='qfq')
get_autype_list(code_list)
get_market_snapshot(code_list)
```

##订阅接口
```python
subscribe(stock_code, data_type) 
unsubscribe(stock_code, data_type) 
query_subscription() 
```


##高频接口
```python
get_stock_quote(code_list)
get_rt_ticker(code, num)
get_cur_kline(code, num, ktype=' K_DAY', autype='qfq') 
get_order_book(code)
```



#接口说明


##参数类型定义

**市场标识** market，（字符串类型）：
| 股票市场 | 标识          |
| ---- | ----------- |
| 港股   | "HK"        |
| 美股   | "US"        |
| 沪股   | "SH"        |
| 深股   | "SZ"        |
| 香港期货 | "HK_FUTURE" |



**证券类型** stock_type，（字符串类型）： 
| 股票类型  | 标识        |
| ----- | --------- |
| 正股    | "STOCK"   |
| 指数    | "IDX"     |
| ETF基金 | "ETF"     |
| 涡轮牛熊  | "WARRANT" |
| 债券    | "BOND"    |



**复权类型**autype，（字符串类型）：
| 复权类型 | 标识    |
| ---- | ----- |
| 前复权  | "qfq" |
| 后复权  | "hfq" |
| 不复权  | None  |



**K线类型**，（字符串类型）：
| K线类型 | 标识       |
| ---- | -------- |
| 1分K  | "K_1M"   |
| 5分K  | "K_5M"   |
| 15分K | "K_15M"  |
| 30分K | "K_30M"  |
| 60分K | "K_60M"  |
| 日K   | "K_DAY"  |
| 周K   | "K_WEEK" |
| 月K   | "K_MON"  |



**订阅数据类型**，（字符串类型） 
| 订阅类型 | 标识           |
| ---- | ------------ |
| 逐笔   | "TICKER"     |
| 报价   | "QUOTE"      |
| 摆盘   | "ORDER_BOOK" |
| 1分K  | "K_1M"       |
| 5分K  | "K_5M"       |
| 15分K | "K_15M"      |
| 30分K | "K_30M"      |
| 60分K | "K_60M"      |
| 日K   | "K_DAY"      |
| 周K   | "K_WEEK"     |
| 月K   | "K_MON"      |



**股票代码模式**为:** 市场+原始代码**
例如，“HK.00700”, “SZ.000001”, “US.AAPL”



**注意，原始代码部分的字符串必须和客户端显示的完全匹配**，比如：
腾讯为“HK.00700”，而不能是“HK.700”



**对于用户来说接口会返回两个值**
ret_code：调用执行返回状态，0为成功，其它为失败
ret_data：
| ret_code | ret_data         |
| -------- | ---------------- |
| 成功       | ret_data为实际数据    |
| 失败       | ret_data为错误描述字符串 |






##详细说明

###get_trading_days

```python
ret_code, ret_data = get_trading_days(market, start_date=None, end_date=None)
```

**功能**：取指定市场，某个日期时间段的交易日列表


**参数**：
**market**: 市场标识
**start_date**: 起始日期;  string类型，格式YYYY-MM-DD，仅指定到日级别即可，默认值None表示最近一年前的日期
**end_date**: 结束日期;  string类型，格式YYYY-MM-DD，仅指定到日级别即可，取默认值None表示取当前日期


**返回**：
ret_code失败时，ret_data返回为错误描述字符串；
客户端无数据时，ret_code为成功，ret_data返回None 
正常情况下，ret_data为日期列表（每个日期是string类型），如果指定时间段中无交易日，则ret_data为空列表。

**失败情况**：
1. 市场标识不合法
2. 起止日期输入不合法
3. 客户端内部或网络错误





### get_stock_basicinfo 
```python
ret_code, ret_data = get_stock_basicinfo(market, stock_type='STOCK')
```


**功能**：取符合市场和股票类型条件的股票简要信息

**参数**：
**market**: 市场标识, string，例如，”HK”，”US”；具体见市场标识说明
**stock_type**: 证券类型, string, 例如，”STOCK”，”ETF”；具体见证券类型说明

**返回**：
ret_code失败时，ret_data返回为错误描述字符串；
客户端无符合条件数据时，ret_code为成功，ret_data返回None 
正常情况下，ret_data为一个dataframe，其中包括：
code：股票代码，string，例如： ”HK.00700”，“US.AAPL”
name：股票名称，string
lot_size：每手股数，int
stock_type：股票类型，string，例如： ”STOCK”，”ETF”

**失败情况**：
1. 市场或股票类型不合法 
2. 客户端内部或网络错误




###get_autype_list


```python
ret_code, ret_data = get_autype_list(code_list)
```

**功能**：获取复权因子数据 
**参数**：
**code_list**: 股票代码列表，例如，HK.00700，US.AAPL
**返回**：
ret_code失败时，ret_data返回为错误描述字符串；
客户端无符合条件数据时，ret_code为成功，ret_data返回None 
正常情况下，ret_data为一个dataframe，其中包括：
**code**：股票代码，string，例如： ”HK.00700”，“US.AAPL”
**ex_div_date**：除权除息日，string类型，格式YYYY-MM-DD 
**split_ratio**：拆合股比例 double，例如，对于5股合1股为1/5，对于1股拆5股为5/1
**per_cash_div**：每股派现；double
**per_share_div_ratio**：每股送股比例；	double
**per_share_trans_ratio**：每股转增股比例；	double
**allotment_ratio**：	每股配股比例；double
**allotment_price**：配股价；double
**stk_spo_ratio**： 增发比例：double
**stk_spo_price**  增发价格：double
**forward_adj_factorA**：前复权因子A；double
**forward_adj_factorB**：前复权因子B；double
**backward_adj_factorA**：后复权因子A；double
**backward_adj_factorB**：后复权因子B；double
返回数据中不一定包含所有codelist中的代码，调用方自己需要检查，哪些股票代码是没有返回复权数据的，未返回复权数据的股票说明没有找到相关信息。

**复权价格 = 复权因子A  * 价格 + 复权因子B**


**失败情况**：
1．	Codelist中股票代码不合法
2．	客户端内部或网络错误




###get_history_kline
```python
ret_code, ret_data = get_history_kline(code, start='', end='', ktype='K_DAY', autype='qfq')
```

**功能**： 获取指定股票K线历史数据
**参数**：
**code**：股票代码
**start** ：开始时间, string; YYYY-MM-DD；为空时取去当前时间;
**end**  ： 结束时间, string; YYYY-MM-DD；为空时取当前时间;
**ktype** ：k线类型，默认为日K
**autype**  复权类型，string；”qfq”-前复权，”hfq”-后复权，None-不复权，默认为”qfq”

开始结束时间按照闭区间查询，时间查询以k线时间time_key作为比较标准。即满足
start<=Time_key<=end条件的k线作为返回内容，k线时间time_key的设定标准在返回值中说明

**返回**：
ret_code失败时，ret_data返回为错误描述字符串；
客户端无符合条件数据时，ret_code为成功，返回None 

正常情况下返回K线数据为一个DataFrame包含:
**code**： 股票代码；string
**time_key**：  K线时间 string  “YYYY-MM-DD HH:mm:ss”
**open**： 开盘价；double
**high**： 最高价；double
**close**： 收盘价；double
**low**： 最低价：double
**volume**： 成交量；long
**turnover** ：成交额；double

对于日K线，time_key为当日时间具体到日，比如说2016-12-23日的日K，K线时间为” 2016-12-23 00:00:00”

对于分K线，time_key为当日时间具体到分，例如，
1分K，覆盖时间为9：35：00到9：35：59的分K，K线时间time_key为”2016-12-23 09:36:00”
5分K，覆盖时间为10：05：00到10：09：59的5分K，K线时间time_key为”2016-12-23 10:10:00”
15分K，覆盖时间为10：00：00到10：14：59的15分K，K线时间time_key为”2016-12-23 10:15:00”
30分K，覆盖时间为10：00：00到10：29：59的30分K，K线时间time_key为” 2016-12-23 10:30:00”
60分K，覆盖时间为10：30：00到11：29：59的60分K，K线时间time_key为” 2016-12-23 11:30:00”

对于周K线，12月19日到12月25日的周K线，K线时间time_key为” 2016-12-19 00:00:00” 

对于月K线，12月的月K线时间time_key为” 2016-12-01 00:00:00”，即为当月1日时间

**失败情况**:
1. 股票代码不合法
2. PLS接口返回错误 





###get_market_snapshot

```python
ret_code, ret_data = get_market_snapshot(code_list):
```

**功能**：一次性获取最近当前股票列表的快照数据（每日变化的信息），该接口对股票个数有限制，一次最多传入200只股票
**参数**：
**code_list**: 股票代码列表，例如，HK.00700，US.AAPL
传入的codelist只允许包含1种股票类型。
**返回**：
ret_code失败时，ret_data返回为错误描述字符串；
客户端无符合条件数据时，ret_code为成功，ret_data返回None 
正常情况下，ret_data为一个dataframe，其中包括：

**code** ：股票代码；string
**data_date**：	交易日期(yyyy-MM-dd)；string
**data_time**：	交易时间'HH:mm:ss'；string
**last_price** ： 	 最新价格；double
**open_price**：	 今日开盘价；double
**high_price**：	 最高价格；double
**low_price**：	     最低价格；double
**prev_close_price**：	昨收盘价格；double
**volume**：	成交数量； long
**turnover**：	成交金额；double
**turnover_rate**：	换手率；double
**amplitude**：	  振幅；double 
**suspension**：	  是否停牌(True表示停牌)；bool
**listing_date** ：  上市日期 (yyyy-MM-dd)；string

返回DataFrame，包含上述字段

返回数据量不一定与codelist长度相等， 用户需要自己判断

**调用频率限制：**
**5s一次**

**失败情况**:
1. Codelist中股票代码不合法
2. Codelist长度超过规定数量
3. 客户端内部或网络错误
4. 传入的codelist包含多种股票类型







###subscribe
```python
ret_code,ret_data= subscribe(stock_code, data_type) 
```


**功能**：订阅注册需要的实时信息，指定股票和订阅的数据类型即可： 
**参数**：
**stock_code**: 需要订阅的股票代码
**data_type**: 需要订阅的数据类型

**返回**：
ret_code失败时，ret_data返回为错误描述字符串；
ret_code为成功，ret_data返回None 
如果指定内容已订阅，则直接返回成功

**失败情况**:
1. 股票代码不合法，不存在
2. 数据类型不合法
3. 订阅额度已满，参考订阅额度表
4. 客户端内部或网络错误




### unsubscribe

```python
ret_code,ret_data = unsubscribe(stock_code, data_type) 
```
ret_code,ret_data = unsubscribe(stock_code, data_type) 
**功能**：退订注册的实时信息，指定股票和订阅的数据类型即可： 
**参数**：
**stock_code**: 需要退订的股票代码
**data_type**: 需要退订的数据类型

**返回**：
ret_code失败时，ret_data返回为错误描述字符串；
ret_code为成功，ret_data返回None 
如果指定内容已退订，则直接返回成功


**失败情况**:
1. 股票代码不合法，不存在
2. 数据类型不合法
3. 内容订阅后不超过60s，就退订
4. 客户端内部或网络错误




###query_subscription
```python
ret_data = query_subscription() 
```

**功能**：查询已订阅的实时信息
**返回**：
字典类型，已订阅类型为主键，值为订阅该类型的股票，例如
```python
{ ‘QUOTE’: [‘HK.00700’, ‘SZ.000001’]
  ‘TICKER’: [‘HK.00700’]
  ‘K_1M’: [‘HK.00700’]
  #无股票订阅摆盘和其它类型分K
}
```


**失败情况**:
客户端内部或网络错误




###get_stock_quote
```python
ret_code, ret_data = get_stock_quote(code_list)
```

**功能**：获取订阅股票报价的实时数据，有订阅要求限制 
**参数**：
**code_list**: 股票代码列表，例如，HK.00700，US.AAPL
传入的codelist只允许包含1种股票类型的股票。
必须确保code_list中的股票均订阅成功后才能够执行
**返回**：
ret_code失败时，ret_data返回为错误描述字符串；
客户端无符合条件数据时，ret_code为成功，返回None 
正常情况下，ret_data为一个dataframe，其结构和get_market_snapshot一样

**失败情况**:
1. codelist中股票代码不合法
2. codelist包含未对QUOTE类型订阅的股票
3. 客户端内部或网络错误
4. 传入的codelist包含多种股票类型




###get_rt_ticker
```python
ret_code, ret_data = get_rt_ticker(code, , num=500)
```


**功能**： 获取指定股票的实时逐笔。取最近num个逐笔，
**参数**：
**code**: 股票代码，例如，HK.00700，US.AAPL
**num**: 最近ticker个数(有最大个数限制，最近500个）
**返回**：
ret_code失败时，ret_data为错误描述字符串；
客户端无符合条件数据时，ret为成功，ret_data返回None
通常情况下，返回DataFrame，DataFrame每一行是一个逐笔记录，包含：
**stock_code** 股票代码
**sequence** 逐笔序号
**date** 成交时间；string 
**price** 成交价格；double
**volume** 成交数量（股数）；int
**turnover** 成交金额；double
**ticker_direction** 逐笔方向；int

deal_type:
| 逐笔标识       | 说明   |
| ---------- | ---- |
| TT_BUY     | 外盘   |
| TT_ASK     | 内盘   |
| TT_NEUTRAL | 中性盘  |


返回的逐笔记录个数不一定与num指定的相等，客户端只会返回自己有的数据。

**失败情况**：
1. code不合法
2. code是未对TICKER类型订阅的股票
3. 客户端内部或网络错误






### get_cur_kline

```python
ret_code, ret_data = get_cur_kline(code, num, ktype='K_DAY', autype='qfq')
```



**功能**： 实时获取指定股票最近num个K线数据，最多1000根
**参数**：
**code** 股票代码
**ktype** k线类型，和get_history_kline一样
**autype**  复权类型，string；qfq-前复权，hfq-后复权，None-不复权，默认为qfq

对于实时K线数据最多取最近1000根

**返回**：
ret_code失败时，ret_data为错误描述字符串；
客户端无符合条件数据时，ret为成功，ret_data返回None
通常情况下，返回DataFrame，DataFrame内容和get_history_kline一样

**失败情况**：
1. code不合法
2. 该股票未对指定K线类型订阅
3. 客户端内部或网络错误




###get_order_book

```python
ret_code, ret_data = get_order_book(code) 
```

**功能**：获取实时摆盘数据
**参数**：
**code**: 股票代码，例如，HK.00700，US.AAPL
**返回**：
ret_code失败时，ret_data为错误描述字符串；
客户端无符合条件数据时，ret为成功，ret_data返回None
通常情况下，返回字典
```python
{‘stock_code’: stock_code
 ‘Ask’:[ (ask_price1, ask_volume1，order_num), (ask_price2, ask_volume2, order_num),…]
‘Bid’: [ (bid_price1, bid_volume1, order_num), (bid_price2, bid_volume2, order_num),…]
}
```

**失败情况**：
1. code不合法
2. 该股票未对ORDER_BOOK类型订阅
3. 客户端内部或网络错误





