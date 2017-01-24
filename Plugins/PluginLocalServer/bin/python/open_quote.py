# -*- coding: utf-8 -*-

import sys
import json
from datetime import datetime
from datetime import timedelta
import pandas as pd
import socket as sock

SRC_ADDR = ('127.0.0.1', 11111)

'''
 parameter relation Mappings between PLS and Python programs
'''
mkt_map = {"HK": 1,
           "US": 2,
           "SH": 3,
           "SZ": 4,
           "HK_FUTURE": 6
           }
rev_mkt_map = {mkt_map[x]: x for x in mkt_map}


sec_type_map = {"STOCK": 3,
                "IDX": 6,
                "ETF": 4,
                "WARRANT": 5,
                "BOND": 1
                }
rev_sec_type_map = {sec_type_map[x]: x for x in sec_type_map}


subtype_map = {"TICKER": 4,
               "QUOTE":  1,
               "ORDER_BOOK": 2,
               "K_1M":    11,
               "K_5M":     7,
               "K_15M":    8,
               "K_30M":    9,
               "K_60M":   10,
               "K_DAY":    6,
               "K_WEEK":  12,
               "K_MON":   13
               }
rev_subtype_map = {subtype_map[x]: x for x in subtype_map}


ktype_map = {"K_1M":     1,
             "K_5M":     6,
             "K_15M":    7,
             "K_30M":    8,
             "K_60M":    9,
             "K_DAY":    2,
             "K_WEEK":   3,
             "K_MON":    4
             }

rev_ktype_map = {ktype_map[x]: x for x in ktype_map}

autype_map = {None: 0,
              "qfq": 1,
              "hfq": 2
              }

rev_autype_map = {autype_map[x]: x for x in autype_map}


ticker_direction = {"TT_BUY": 1,
                    "TT_SELL": 2,
                    "TT_NEUTRAL": 3
                    }

rev_ticker_direction = {ticker_direction[x]: x for x in ticker_direction}


RET_OK = 0
RET_ERROR = -1

ERROR_STR_PREFIX = 'ERROR. '


def _check_date_str_format(s):
    try:
        _ = datetime.strptime(s, "%Y-%m-%d")
        return RET_OK, None
    except ValueError:
        err = sys.exc_info()[1]
        error_str = ERROR_STR_PREFIX + str(err)
        return RET_ERROR, error_str


def _extract_pls_rsp(rsp_str):
    try:
        rsp = json.loads(rsp_str)
    except ValueError:
        err = sys.exc_info()[1]
        err_str = ERROR_STR_PREFIX + str(err)
        return RET_ERROR, err_str, None

    error_code = int(rsp['ErrCode'])

    if error_code != 0:
        error_str = ERROR_STR_PREFIX + rsp['ErrDesc']
        return RET_ERROR, error_str, None

    if 'RetData' not in rsp:
        error_str = ERROR_STR_PREFIX + 'No ret data found in client rsp. Response: %s' % rsp
        return RET_ERROR, error_str, None

    return RET_OK, "", rsp


def _normalize_date_format(date_str):
    date_obj = datetime.strptime(date_str, "%Y-%m-%d")
    ret = date_obj.strftime("%Y-%m-%d")
    return ret


def _split_stock_str(stock_str):

    if isinstance(stock_str, str) is False:
        error_str = ERROR_STR_PREFIX + "value of stock_str is %s of type %s, and type %s is expected" \
                                       % (stock_str, type(stock_str), str(str))
        return RET_ERROR, error_str

    split_loc = stock_str.find(".")
    '''do not use the built-in split function in python.
    The built-in function cannot handle some stock strings correctly.
    for instance, US..DJI, where the dot . itself is a part of original code'''
    if 0 <= split_loc < len(stock_str) - 1 and stock_str[0:split_loc] in mkt_map:
        market_str = stock_str[0:split_loc]
        market_code = mkt_map[market_str]
        partial_stock_str = stock_str[split_loc+1:]
        return RET_OK, (market_code, partial_stock_str)

    else:

        error_str = ERROR_STR_PREFIX + "format of %s is wrong. (US.AAPL, HK.00700, SZ.000001)" % stock_str
        return RET_ERROR, error_str


def _merge_stock_str(market, partial_stock_str):
    """
    :param market: market code
    :param partial_stock_str: original stock code string. i.e. "AAPL","00700", "000001"
    :return: unified representation of a stock code. i.e. "US.AAPL", "HK.00700", "SZ.000001"

    """

    market_str = rev_mkt_map[market]
    stock_str = '.'.join([market_str, partial_stock_str])
    return stock_str


def _str2binary(s):
    """
    :param s: string content to be transformed to binary
    :return: binary
    """
    return s.encode('utf-8')


def _binary2str(b):
    """

    :param b: binary content to be transformed to string
    :return: string
    """
    return b.decode('utf-8')


class _NetworkQueryCtx:
    """
    Network query context manages connection between python program and FUTU client program.

    Short (non-persistent) connection can be created by setting long_conn prarameter False, which suggests that
    TCP connection is closed once a query session finished

    Long (persistent) connection can be created by setting long_conn prarameter True,  which suggests that TCP
    connection is persisted after a query session finished, waiting for next query.

    """
    def __init__(self, long_conn=False):
        self.s = None
        self.long_conn = long_conn

    def _create_session(self):
        if self.long_conn is True and self.s is not None:
            return RET_OK, ""

        s = sock.socket(sock.AF_INET, sock.SOCK_STREAM)
        s.setsockopt(sock.SOL_SOCKET, sock.SO_REUSEADDR, 1)
        s.setsockopt(sock.SOL_SOCKET, sock.SO_LINGER, 0)
        s.settimeout(5)
        self.s = s

        try:
            self.s.connect(SRC_ADDR)
        except Exception:
            err = sys.exc_info()[1]
            error_str = ERROR_STR_PREFIX + str(err)
            self._force_close_session()
            return RET_ERROR, error_str

        return RET_OK, ""

    def _force_close_session(self):
        if self.s is None:
            return
        self.s.close()
        self.s = None

    def _close_session(self):

        if self.s is None or self.long_conn is True:
            return
        self.s.close()
        self.s = None

    def network_query(self, req_str):
        """
        the function sends req_str to FUTU client and try to get response from the client.
        :param req_str
        :return: rsp_str
        """

        ret, msg = self._create_session()
        if ret != RET_OK:
            return ret, msg, None

        s_buf = _str2binary(req_str)
        try:
            s_cnt = self.s.send(s_buf)
        except Exception:
            err = sys.exc_info()[1]
            error_str = ERROR_STR_PREFIX + str(err) + 'when sending. For req: ' + req_str

            self._force_close_session()
            return RET_ERROR, error_str, None

        rsp_buf = b''
        while rsp_buf.find(b'\r\n\r\n') < 0:

            try:
                recv_buf = self.s.recv(5 * 1024 * 1024)
                rsp_buf += recv_buf
            except Exception:
                err = sys.exc_info()[1]
                error_str = ERROR_STR_PREFIX + str(
                    err) + 'when recving after sending %s bytes. For req: ' % s_cnt + req_str
                self._force_close_session()
                return RET_ERROR, error_str, None

        rsp_str = _binary2str(rsp_buf)
        self._close_session()
        return RET_OK, "", rsp_str

    def __del__(self):
        if self.s is not None:
            self.s.close()
            self.s = None


class TradeDayQuery:
    """
    Query Conversion for getting trading days.
    """
    def __init__(self):
        pass

    @classmethod
    def pack_req(cls, market, start_date=None, end_date=None):
        """
        Convert from user request for trading days to PLS request
        :param market:
        :param start_date:
        :param end_date:
        :return:  json string for request

        Example:

        ret,msg,content =  TradeDayQuery.pack_req("US", "2017-01-01", "2017-01-18")

        ret: 0
        msg: ""
        content:
        '{"Protocol": "1013", "Version": "1", "ReqParam": {"end_date": "2017-01-18",
        "Market": "2", "start_date": "2017-01-01"}}\r\n'

        """

        # '''Parameter check'''
        if market not in mkt_map:
            error_str = ERROR_STR_PREFIX + " market is %s, which is not valid. (%s)" \
                                           % (market, ",".join([x for x in mkt_map]))
            return RET_ERROR, error_str, None

        if start_date is None:
            today = datetime.today()
            start = today - timedelta(days=365)

            start_date = start.strftime("%Y-%m-%d")
        else:
            ret, msg = _check_date_str_format(start_date)
            if ret != RET_OK:
                return ret, msg, None
            start_date = _normalize_date_format(start_date)

        if end_date is None:
            today = datetime.today()
            end_date = today.strftime("%Y-%m-%d")
        else:
            ret, msg = _check_date_str_format(end_date)
            if ret != RET_OK:
                return ret, msg, None
            end_date = _normalize_date_format(end_date)

        # pack to json
        mkt_str = str(mkt_map[market])
        req = {"Protocol": "1013",
               "Version": "1",
               "ReqParam": {"Market": mkt_str,
                            "start_date": start_date,
                            "end_date": end_date
                            }
               }
        req_str = json.dumps(req) + '\r\n'
        return RET_OK, "", req_str

    @classmethod
    def unpack_rsp(cls, rsp_str):
        """
        Convert from PLS response to user response
        :param rsp_str:
        :return: trading day list

        Example:

        rsp_str : '{"ErrCode":"0","ErrDesc":"","Protocol":"1013","RetData":{"Market":"2",
        "TradeDateArr":["2017-01-17","2017-01-13","2017-01-12","2017-01-11",
        "2017-01-10","2017-01-09","2017-01-06","2017-01-05","2017-01-04",
        "2017-01-03"],"end_date":"2017-01-18","start_date":"2017-01-01"},"Version":"1"}\n\r\n\r\n'

         ret,msg,content = TradeDayQuery.unpack_rsp(rsp_str)

         ret : 0
         msg : ""
         content : ['2017-01-17',
                    '2017-01-13',
                    '2017-01-12',
                    '2017-01-11',
                    '2017-01-10',
                    '2017-01-09',
                    '2017-01-06',
                    '2017-01-05',
                    '2017-01-04',
                    '2017-01-03']

        """
        # response check and unpack response json to objects
        ret, msg, rsp = _extract_pls_rsp(rsp_str)
        if ret != RET_OK:
            return RET_ERROR, msg, None

        rsp_data = rsp['RetData']

        if 'TradeDateArr' not in rsp_data:
            error_str = ERROR_STR_PREFIX + "cannot find TradeDateArr in client rsp. Response: %s" % rsp_str
            return RET_ERROR, error_str, None

        raw_trading_day_list = rsp_data['TradeDateArr']

        if raw_trading_day_list is None or len(raw_trading_day_list) == 0:
            return RET_OK, "", []

        # convert to list format that we use
        trading_day_list = [_normalize_date_format(x) for x in raw_trading_day_list]

        return RET_OK, "", trading_day_list


class StockBasicInfoQuery:

    def __init__(self):
        pass

    @classmethod
    def pack_req(cls, market, stock_type='STOCK'):
        """

        :param market:
        :param stock_type:
        :return: json string for request

        Example:
         ret,msg,content = StockBasicInfoQuery.pack_req("HK_FUTURE","IDX")

         ret : 0
         msg : ""
         content : '{"Protocol": "1014", "Version": "1", "ReqParam": {"Market": "6", "StockType": "6"}}\r\n'
        """
        if market not in mkt_map:
            error_str = ERROR_STR_PREFIX + " market is %s, which is not valid. (%s)" \
                                           % (market, ",".join([x for x in mkt_map]))
            return RET_ERROR, error_str, None

        if stock_type not in sec_type_map:
            error_str = ERROR_STR_PREFIX + " stock_type is %s, which is not valid. (%s)" \
                                           % (stock_type, ",".join([x for x in sec_type_map]))
            return RET_ERROR, error_str, None

        mkt_str = str(mkt_map[market])
        stock_type_str = str(sec_type_map[stock_type])
        req = {"Protocol": "1014",
               "Version": "1",
               "ReqParam": {"Market": mkt_str,
                            "StockType": stock_type_str,
                            }
               }
        req_str = json.dumps(req) + '\r\n'
        return RET_OK, "", req_str

    @classmethod
    def unpack_rsp(cls, rsp_str):
        """

        :param rsp_str:
        :return: json string for request

        Example:

        rsp_str : '{"ErrCode":"0","ErrDesc":"","Protocol":"1014",
        "RetData":{"BasicInfoArr":
        [{"LotSize":"0","Name":"恒指当月期货","StockCode":"999010","StockID":"999010","StockType":"6"},
        {"LotSize":"0","Name":"恒指下月期货","StockCode":"999011","StockID":"999011","StockType":"6"}],
        "Market":"6"},"Version":"1"}\n\r\n\r\n'


         ret,msg,content = StockBasicInfoQuery.unpack_rsp(rsp_str)

        ret : 0
        msg : ""
        content : [{'code': 'HK_FUTURE.999010',
                    'lot_size': 0,
                    'name': '恒指当月期货',
                    'stock_type': 'IDX'},
                   {'code': 'HK_FUTURE.999011',
                    'lot_size': 0,
                    'name': '恒指下月期货',
                    'stock_type': 'IDX'}]

        """
        ret, msg, rsp = _extract_pls_rsp(rsp_str)
        if ret != RET_OK:
            return RET_ERROR, msg, None

        rsp_data = rsp['RetData']

        if 'BasicInfoArr' not in rsp_data:
            error_str = ERROR_STR_PREFIX + "cannot find BasicInfoArr in client rsp. Response: %s" % rsp_str
            return RET_ERROR, error_str, None

        raw_basic_info_list = rsp_data["BasicInfoArr"]
        market = rsp_data["Market"]

        if raw_basic_info_list is None or len(raw_basic_info_list) == 0:
            return RET_OK, "", []

        basic_info_list = [{"code": _merge_stock_str(int(market), record['StockCode']),
                            "name": record["Name"],
                            "lot_size": int(record["LotSize"]),
                            "stock_type": rev_sec_type_map[int(record["StockType"])]
                            }
                           for record in raw_basic_info_list]
        return RET_OK, "", basic_info_list


class MarketSnapshotQuery:

    def __init__(self):
        pass

    @classmethod
    def pack_req(cls):
        pass

    @classmethod
    def unpack_rsp(cls):
        pass


class SubscriptionQuery:

    def __init__(self):
        pass

    @classmethod
    def pack_subscribe_req(cls, stock_str, data_type):
        """
        :param stock_str:
        :param data_type:
        :return:
        """
        ret, content = _split_stock_str(stock_str)
        if ret == RET_ERROR:
            error_str = content
            return RET_ERROR, error_str, None

        market_code, stock_code = content

        if data_type not in subtype_map:
            subtype_str = ','.join([x for x in subtype_map])
            error_str = ERROR_STR_PREFIX + 'data_type is %s , which is wrong. (%s)' % (data_type, subtype_str)
            return RET_ERROR, error_str, None

        subtype = subtype_map[data_type]
        req = {"Protocol": "1005",
               "Version": "1",
               "ReqParam": {"Market": str(market_code),
                            "StockCode": stock_code,
                            "StockSubType": str(subtype)
                            }
               }
        req_str = json.dumps(req) + '\r\n'
        return RET_OK, "", req_str

    @classmethod
    def unpack_subscribe_rsp(cls, rsp_str):

        ret, msg, content = _extract_pls_rsp(rsp_str)
        if ret != RET_OK:
            return RET_ERROR, msg, None

        return RET_OK, "", None

    @classmethod
    def pack_unsubscribe_req(cls, stock_str, data_type):
        ret, content = _split_stock_str(stock_str)
        if ret == RET_ERROR:
            error_str = content
            return RET_ERROR, error_str, None

        market_code, stock_code = content

        if data_type not in subtype_map:
            subtype_str = ','.join([x for x in subtype_map])
            error_str = ERROR_STR_PREFIX + 'data_type is %s, which is wrong. (%s)' % (data_type, subtype_str)
            return RET_ERROR, error_str, None

        subtype = subtype_map[data_type]

        req = {"Protocol": "1006",
               "Version": "1",
               "ReqParam": {"Market": str(market_code),
                            "StockCode": stock_code,
                            "StockSubType": str(subtype)
                            }
               }
        req_str = json.dumps(req) + '\r\n'
        return RET_OK, "", req_str

    @classmethod
    def unpack_unsubscribe_rsp(cls, rsp_str):
        ret, msg, content = _extract_pls_rsp(rsp_str)
        if ret != RET_OK:
            return RET_ERROR, msg, None

        return RET_OK, "", None

    @classmethod
    def pack_subscription_query_req(cls):
        req = {"Protocol": "1007",
               "Version": "1",
               "ReqParam": {}
               }
        req_str = json.dumps(req) + '\r\n'
        return RET_OK, "", req_str

    @classmethod
    def unpack_subscription_query_rsp(cls, rsp_str):
        ret, msg, rsp = _extract_pls_rsp(rsp_str)
        if ret != RET_OK:
            return RET_ERROR, msg, None

        rsp_data = rsp['RetData']

        if 'SubInfoArr' not in rsp_data:
            error_str = ERROR_STR_PREFIX + "cannot find TradeDateArr in client rsp. Response: %s" % rsp_str
            return RET_ERROR, error_str, None

        subscription_table = {}

        raw_subscription_list = rsp_data['SubInfoArr']
        if raw_subscription_list is None or len(raw_subscription_list) == 0:
            return RET_OK, "", subscription_table

        subscription_list = [(_merge_stock_str(int(x['Market']), x['StockCode']),
                              rev_subtype_map[int(x['StockSubType'])])
                             for x in raw_subscription_list]

        for stock_code_str, sub_type in subscription_list:
            if sub_type not in subscription_table:
                subscription_table[sub_type] = []
            subscription_table[sub_type].append(stock_code_str)

        return RET_OK, "", subscription_table


class StockQuoteQuery:
    def __init__(self):
        pass

    @classmethod
    def pack_req(cls, stock_list):
        """

        :param stock_list:
        :return:
        """
        stock_tuple_list = []
        failure_tuple_list = []
        for stock_str in stock_list:
            ret_code, content = _split_stock_str(stock_str)
            if ret_code != RET_OK:
                msg = content
                error_str = ERROR_STR_PREFIX + msg
                failure_tuple_list.append((ret_code, error_str))
                continue

            market_code, stock_code = content
            stock_tuple_list.append((str(market_code), stock_code))

        if len(failure_tuple_list) > 0:
            error_str = '\n'.join([x[1] for x in failure_tuple_list])
            return RET_ERROR, error_str, None

        req = {"Protocol": "1023",
               "Version": "1",
               "ReqParam": {'ReqArr': [{'Market': stock[0], 'StockCode': stock[1]} for stock in stock_tuple_list]}
               }

        req_str = json.dumps(req) + '\r\n'
        return RET_OK, "", req_str

    @classmethod
    def unpack_rsp(cls, rsp_str):
        ret, msg, rsp = _extract_pls_rsp(rsp_str)
        if ret != RET_OK:
            return RET_ERROR, msg, None

        rsp_data = rsp['RetData']
        if "SubSnapshotArr" not in rsp_data:
            error_str = ERROR_STR_PREFIX + "cannot find SubSnapshotArr in client rsp. Response: %s" % rsp_str
            return RET_ERROR, error_str, None

        raw_quote_list = rsp_data["SubSnapshotArr"]

        quote_list = [{'code': _merge_stock_str(int(record['Market']), record['StockCode']),
                       'data_date': record['Date'],
                       'data_time': record['Time'],
                       'last_price': float(record['Cur'])/1000,
                       'open_price': float(record['Open'])/1000,
                       'high_price': float(record['High'])/1000,
                       'low_price': float(record['Low'])/1000,
                       'prev_close_price': float(record['LastClose'])/1000,
                       'volume': int(record['TDVol']),
                       'turnover':  float(record['TDVal'])/1000,
                       'turnover_rate': float(record['Turnover'])/1000,
                       'amplitude': float(record['Amplitude'])/1000,
                       'suspension': True if int(record['Suspension']) != 2 else False,
                       'listing_date': record['ListTime']
                       }
                      for record in raw_quote_list]

        return RET_OK, "", quote_list


class TickerQuery:

    def __init__(self):
        pass

    @classmethod
    def pack_req(cls, stock_str, num=500):
        ret, content = _split_stock_str(stock_str)
        if ret == RET_ERROR:
            error_str = content
            return RET_ERROR, error_str, None

        if isinstance(num, int) is False:
            error_str = ERROR_STR_PREFIX + "num is %s of type %s, and the type shoud be %s" \
                                           % (num, str(type(num)), str(int))
            return RET_ERROR, error_str, None

        if num < 0:
            error_str = ERROR_STR_PREFIX + "num is %s, which is less than 0" % num
            return RET_ERROR, error_str, None

        market_code, stock_code = content

        req = {"Protocol": "1012",
               "Version": "1",
               "ReqParam": {'Market': str(market_code),
                            'StockCode': stock_code,
                            "Sequence": str(-1),
                            'Num': str(num)
                            }
               }

        req_str = json.dumps(req) + '\r\n'
        return RET_OK, "", req_str

    @classmethod
    def unpack_rsp(cls, rsp_str):
        ret, msg, rsp = _extract_pls_rsp(rsp_str)
        if ret != RET_OK:
            return RET_ERROR, msg, None

        rsp_data = rsp['RetData']
        if "TickerArr" not in rsp_data:
            error_str = ERROR_STR_PREFIX + "cannot find TickerArr in client rsp. Response: %s" % rsp_str
            return RET_ERROR, error_str, None

        raw_ticker_list = rsp_data["TickerArr"]
        if raw_ticker_list is None or len(raw_ticker_list) == 0:
            return RET_OK, "", []

        stock_code = _merge_stock_str(int(rsp_data['Market']), rsp_data['StockCode'])
        ticker_list = [{"stock_code": stock_code,
                        "time":  record['Time'],
                        "price": float(record['Price'])/1000,
                        "volume": record['Volume'],
                        "turnover": float(record['Turnover'])/1000,
                        "ticker_direction": rev_ticker_direction[int(record['Direction'])],
                        "sequence": int(record["Sequence"])
                        }
                       for record in raw_ticker_list]
        return RET_OK, "", ticker_list


class CurKlineQuery:

    def __init__(self):
        pass

    @classmethod
    def pack_req(cls, stock_str, num, ktype='K_DAY', autype='qfq'):
        ret, content = _split_stock_str(stock_str)
        if ret == RET_ERROR:
            error_str = content
            return RET_ERROR, error_str, None

        market_code, stock_code = content

        if ktype not in ktype_map:
            error_str = ERROR_STR_PREFIX + "ktype is %s, which is not valid. (%s)" \
                                           % (ktype, ", ".join([x for x in ktype_map]))
            return RET_ERROR, error_str, None

        if autype not in autype_map:
            error_str = ERROR_STR_PREFIX + "autype is %s, which is not valid. (%s)" \
                                           % (autype, ", ".join([str(x) for x in autype_map]))
            return RET_ERROR, error_str, None

        if isinstance(num, int) is False:
            error_str = ERROR_STR_PREFIX + "num is %s of type %s, which type shoud be %s" \
                                           % (num, str(type(num)), str(int))
            return RET_ERROR, error_str, None

        if num < 0:
            error_str = ERROR_STR_PREFIX + "num is %s, which is less than 0" % num
            return RET_ERROR, error_str, None

        req = {"Protocol": "1011",
               "Version": "1",
               "ReqParam": {'Market': str(market_code),
                            'StockCode': stock_code,
                            'Num': str(num),
                            'KLType': str(ktype_map[ktype]),
                            'RehabType': str(autype_map[autype])
                            }
               }

        req_str = json.dumps(req) + '\r\n'
        return RET_OK, "", req_str

    @classmethod
    def unpack_rsp(cls, rsp_str):
        ret, msg, rsp = _extract_pls_rsp(rsp_str)
        if ret != RET_OK:
            return RET_ERROR, msg, None

        rsp_data = rsp['RetData']

        if "KLDataArr" not in rsp_data:
            error_str = ERROR_STR_PREFIX + "cannot find KLDataArr in client rsp. Response: %s" % rsp_str
            return RET_ERROR, error_str, None

        if rsp_data["KLDataArr"] is None or len(rsp_data["KLDataArr"]) == 0:
            return RET_OK, "", []

        raw_kline_list = rsp_data["KLDataArr"]
        stock_code = _merge_stock_str(int(rsp_data['Market']), rsp_data['StockCode'])
        kline_list = [{"code": stock_code,
                       "time_key": record['Time'],
                       "open": float(record['Open'])/1000,
                       "high": float(record['High'])/1000,
                       "low": float(record['Low'])/1000,
                       "close": float(record['Close'])/1000,
                       "volume": record['TDVol'],
                       "turnover": float(record['TDVal'])/1000
                       }
                      for record in raw_kline_list]

        return RET_OK, "", kline_list


class OrderBookQuery:

    def __init__(self):
        pass

    @classmethod
    def pack_req(cls, stock_str):
        ret, content = _split_stock_str(stock_str)
        if ret == RET_ERROR:
            error_str = content
            return RET_ERROR, error_str, None

        market_code, stock_code = content
        req = {"Protocol": "1002",
               "Version": "1",
               "ReqParam": {'Market': str(market_code), 'StockCode': stock_code, 'Num': str(10)}
               }
        req_str = json.dumps(req) + '\r\n'
        return RET_OK, "", req_str

    @classmethod
    def unpack_rsp(cls, rsp_str):
        ret, msg, rsp = _extract_pls_rsp(rsp_str)
        if ret != RET_OK:
            return RET_ERROR, msg, None

        rsp_data = rsp['RetData']
        if "GearArr" not in rsp_data:
            error_str = ERROR_STR_PREFIX + "cannot find GearArr in client rsp. Response: %s" % rsp_str
            return RET_ERROR, error_str, None

        raw_order_book = rsp_data["GearArr"]
        stock_str = _merge_stock_str(int(rsp_data['Market']), rsp_data['StockCode'])

        order_book = {'stock_code': stock_str, 'Ask': [], 'Bid': []}

        for record in raw_order_book:
            bid_record = (float(record['BuyPrice'])/1000, int(record['BuyVol']), int(record['BuyOrder']))
            ask_record = (float(record['SellPrice'])/1000, int(record['SellVol']), int(record['SellOrder']))

            order_book['Bid'].append(bid_record)
            order_book['Ask'].append(ask_record)

        return RET_OK, "", order_book


_query_map = {"Ticker": {"long_conn": True, "ctx": None},
              "CurKline":  {"long_conn": True, "ctx": None},
              "StockQuote": {"long_conn": True, "ctx": None},
              "OrderBook":  {"long_conn": True, "ctx": None}
              }


def _get_query_processor(pack_func, unpack_func, query_type=None):
    if query_type in _query_map:
        long_conn = True
        net_query_ctx = _query_map[query_type]["ctx"]
        if net_query_ctx is None:
            net_query_ctx = _NetworkQueryCtx(long_conn)
            _query_map[query_type]["ctx"] = net_query_ctx
    else:
        long_conn = False
        net_query_ctx = _NetworkQueryCtx(long_conn)

    _network_query = net_query_ctx.network_query

    def _query_processor(**kargs):
        ret_code, msg, req_str = pack_func(**kargs)
        if ret_code == RET_ERROR:
            return ret_code, msg, None

        ret_code, msg, rsp_str = _network_query(req_str)

        if ret_code == RET_ERROR:
            return ret_code, msg, None

        ret_code, msg, content = unpack_func(rsp_str)
        if ret_code == RET_ERROR:
            return ret_code, msg, None
        return RET_OK, msg, content

    return _query_processor


def get_trading_days(market, start_date=None, end_date=None):

    query_processor = _get_query_processor(TradeDayQuery.pack_req,
                                           TradeDayQuery.unpack_rsp)

    # the keys of kargs should be corresponding to the actual function arguments
    kargs = {'market': market, 'start_date': start_date, "end_date": end_date}
    ret_code, msg, trade_day_list = query_processor(**kargs)

    if ret_code != RET_OK:
        return RET_ERROR, msg

    return RET_OK, trade_day_list


def get_stock_basicinfo(market, stock_type='STOCK'):

    query_processor = _get_query_processor(StockBasicInfoQuery.pack_req,
                                           StockBasicInfoQuery.unpack_rsp)
    kargs = {"market": market, 'stock_type': stock_type}

    ret_code, msg, basic_info_list = query_processor(**kargs)
    if ret_code == RET_ERROR:
        return ret_code, msg

    col_list = ['code', 'name', 'lot_size', 'stock_type']

    basic_info_table = pd.DataFrame(basic_info_list, columns=col_list)

    return RET_OK, basic_info_table


def get_history_kline(code, start='', end='', ktype='K_DAY', autype='qfq'):
    _ = code, start, end, ktype, autype
    pass


def get_autype_list(code_list):
    _ = code_list
    pass


def get_market_snapshot(code_list):
    _ = code_list
    pass


def subscribe(stock_code, data_type):
    """
    subcribe a sort of data for a stock
    :param stock_code: string stock_code . For instance, "HK.00700", "US.AAPL"
    :param data_type: string  data type. For instance, "K_1M", "K_MON"
    :return: (ret_code, ret_data). ret_code: RET_OK or RET_ERROR.
    """
    query_processor = _get_query_processor(SubscriptionQuery.pack_subscribe_req,
                                           SubscriptionQuery.unpack_subscribe_rsp)

    # the keys of kargs should be corresponding to the actual function arguments
    kargs = {'stock_str': stock_code, 'data_type': data_type}
    ret_code, msg, _ = query_processor(**kargs)

    if ret_code != RET_OK:
        return RET_ERROR, msg

    return RET_OK, None


def unsubscribe(stock_code, data_type):
    """
    unsubcribe a sort of data for a stock
    :param stock_code: string stock_code . For instance, "HK.00700", "US.AAPL"
    :param data_type: string  data type. For instance, "K_1M", "K_MON"
    :return: (ret_code, ret_data). ret_code: RET_OK or RET_ERROR.
    """
    query_processor = _get_query_processor(SubscriptionQuery.pack_unsubscribe_req,
                                           SubscriptionQuery.unpack_unsubscribe_rsp)
    # the keys of kargs should be corresponding to the actual function arguments
    kargs = {'stock_str': stock_code, 'data_type': data_type}

    ret_code, msg, _ = query_processor(**kargs)

    if ret_code != RET_OK:
        return RET_ERROR, msg

    return RET_OK, None


def query_subscription():
    """
    get the current subscription table
    :return:
    """
    query_processor = _get_query_processor(SubscriptionQuery.pack_subscription_query_req,
                                           SubscriptionQuery.unpack_subscription_query_rsp)

    ret_code, msg, subscription_table = query_processor()
    if ret_code == RET_ERROR:
        return ret_code, msg

    return RET_OK, subscription_table


def get_stock_quote(code_list):
    """
    :param code_list:
    :return: DataFrame of quote data

    Usage:

    After subcribe "QUOTE" type for given stock codes, invoke

    get_stock_quote to obtain the data

    """

    query_processor = _get_query_processor(StockQuoteQuery.pack_req,
                                           StockQuoteQuery.unpack_rsp,
                                           query_type="StockQuote"
                                           )
    kargs = {"stock_list": code_list}

    ret_code, msg, quote_list = query_processor(**kargs)
    if ret_code == RET_ERROR:
        return ret_code, msg

    col_list = ['code', 'data_date', 'data_time', 'last_price', 'open_price',
                'high_price', 'low_price', 'prev_close_price',
                'volume', 'turnover', 'turnover_rate', 'amplitude', 'suspension', 'listing_date'
                ]

    quote_frame_table = pd.DataFrame(quote_list, columns=col_list)

    return RET_OK, quote_frame_table


def get_rt_ticker(code, num=500):
    query_processor = _get_query_processor(TickerQuery.pack_req,
                                           TickerQuery.unpack_rsp,
                                           query_type="Ticker")
    kargs = {"stock_str": code, "num": num}
    ret_code, msg, ticker_list = query_processor(**kargs)
    if ret_code == RET_ERROR:
        return ret_code, msg

    col_list = ['stock_code', 'time', 'price', 'volume', 'turnover', "ticker_direction", 'sequence']
    ticker_frame_table = pd.DataFrame(ticker_list, columns=col_list)

    return RET_OK, ticker_frame_table


def get_cur_kline(code, num, ktype='K_DAY', autype='qfq'):
    query_processor = _get_query_processor(CurKlineQuery.pack_req,
                                           CurKlineQuery.unpack_rsp,
                                           query_type="CurKline")

    kargs = {"stock_str": code, "num": num, "ktype": ktype, "autype": autype}
    ret_code, msg, kline_list = query_processor(**kargs)
    if ret_code == RET_ERROR:
        return ret_code, msg

    col_list = ['code', 'time_key', 'open', 'close', 'high', 'low', 'volume', 'turnover']
    kline_frame_table = pd.DataFrame(kline_list, columns=col_list)

    return RET_OK, kline_frame_table


def get_order_book(code):
    query_processor = _get_query_processor(OrderBookQuery.pack_req,
                                           OrderBookQuery.unpack_rsp,
                                           query_type="OrderBook")

    kargs = {"stock_str": code}
    ret_code, msg, orderbook = query_processor(**kargs)
    if ret_code == RET_ERROR:
        return ret_code, msg

    return RET_OK, orderbook


if __name__ == "__main__":

    # Examples for use the python functions
    #
    def _example_stock_quote():
        stock_code_list = ["US.AAPL", "HK.00700", "SZ.000001"]

        # subscribe "QUOTE"
        for stk_code in stock_code_list:
            ret_status, ret_data = subscribe(stk_code, "QUOTE")
            if ret_status != RET_OK:
                print("%s %s: %s" % (stk_code, "QUOTE", ret_data))
                exit()

        ret_status, ret_data = query_subscription()

        if ret_status == RET_ERROR:
            print(ret_status)
            exit()

        print(ret_data)

        ret_status, ret_data = get_stock_quote(stock_code_list)
        if ret_status == RET_ERROR:
            print(ret_data)
            exit()
        quote_table = ret_data

        print("QUOTE_TABLE")
        print(quote_table)


    def _example_cur_kline():
        # subscribe Kline
        stock_code_list = ["US.AAPL", "HK.00700", "SZ.000001"]
        sub_type_list = ["K_1M", "K_5M", "K_15M", "K_30M", "K_60M", "K_DAY", "K_WEEK", "K_MON"]

        for code in stock_code_list:
            for sub_type in sub_type_list:
                ret_status, ret_data = subscribe(code, sub_type)
                if ret_status != RET_OK:
                    print("%s %s: %s" % (code, sub_type, ret_data))
                    exit()

        ret_status, ret_data = query_subscription()

        if ret_status == RET_ERROR:
            print(ret_data)
            exit()

        print(ret_data)

        for code in stock_code_list:
            for ktype in ["K_DAY", "K_1M", "K_5M"]:
                ret_code, ret_data = get_cur_kline(code, 5, ktype)
                if ret_code == RET_ERROR:
                    print(code, ktype, ret_data)
                    exit()
                kline_table = ret_data
                print("%s KLINE %s" % (code, ktype))
                print(kline_table)
                print("\n\n")


    def _example_rt_ticker():
        stock_code_list = ["US.AAPL", "HK.00700", "SZ.000001", "SH.601318"]

        # subscribe "TICKER"
        for stk_code in stock_code_list:
            ret_status, ret_data = subscribe(stk_code, "TICKER")
            if ret_status != RET_OK:
                print("%s %s: %s" % (stk_code, "TICKER", ret_data))
                exit()

        for stk_code in stock_code_list:
            ret_status, ret_data = get_rt_ticker(stk_code, 10)
            if ret_status == RET_ERROR:
                print(stk_code, ret_data)
                exit()
            print("%s TICKER" % stk_code)
            print(ret_data)
            print("\n\n")


    def _example_order_book():
        stock_code_list = ["US.AAPL", "HK.00700", "SZ.000001", "SH.601318"]

        # subscribe "ORDER_BOOK"
        for stk_code in stock_code_list:
            ret_status, ret_data = subscribe(stk_code, "ORDER_BOOK")
            if ret_status != RET_OK:
                print("%s %s: %s" % (stk_code,  "ORDER_BOOK", ret_data))
                exit()

        for stk_code in stock_code_list:
            ret_status, ret_data = get_order_book(stk_code)
            if ret_status == RET_ERROR:
                print(stk_code, ret_data)
                exit()
            print("%s ORDER_BOOK" % stk_code)
            print(ret_data)
            print("\n\n")


    def _example_get_trade_days():

        ret_status, ret_data = get_trading_days("US", "2017-01-01", "2017-01-18")
        if ret_status == RET_ERROR:
            print(ret_data)
            exit()
        print("TRADING DAYS")
        for x in ret_data:
            print(x)

    def _example_stock_basic():
        ret_status, ret_data = get_stock_basicinfo("US", "STOCK")
        if ret_status == RET_ERROR:
            print(ret_data)
            exit()
        print("stock_basic")
        print(ret_data)

    _example_stock_quote()
    _example_cur_kline()
    _example_rt_ticker()
    _example_order_book()
    _example_get_trade_days()
    _example_stock_basic()
