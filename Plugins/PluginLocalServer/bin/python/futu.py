# -*- coding: utf-8 -*-
# 2015.11.21 FUTU API 第一个版本
# 2015.12.17 增加buy,sell,get_depth,cancel基本函数
# 2015.12.18 验证buy,sell函数模拟单，不过改单和修改单还没验证通过
# 2015.12.22 验证modify,cancel函数模拟单
# ver: 1.0
# developer：zmworm
# public wechat id(微信公众号)：zhaonote
import httplib
import urllib
import json
import socket
import sys
import string

import time
import thread
import threading

import math
import codecs
import traceback

#futnn plubin会开启本地监听服务端 
# 请求及发送数据都是jason格式, 具体详见插件的协议文档 
g_host="localhost"
g_port=11111


class Futu():
	def __init__(self):
		self.s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		self.s.connect((g_host,g_port))
		self.lock = thread.allocate_lock()
	def __del__(self):
		self.s.close()
	def __get_market_code(self,market_name):
		if market_name == 'hk':
			return 1
		if market_name == 'us':
			return 2
		if market_name == 'sh':
			return 3
		if market_name == 'sz':
			return 4
		return market_name
	def __socket_call(self,command,param):
		self.lock.acquire()
		try:
			req = {'Protocol':str(command),\
				'ReqParam': param,\
				'Version':'1'} 		
			mystr = json.dumps(req) + '\n'
			self.s.send(mystr) 
			rsp = ""
			buf = self.s.recv(4096)
			#print buf
			mybuf = buf.split("\r\n")
			for rsp in mybuf:
				if len(rsp) > 2:
					try:
						rsp = rsp.decode('utf-8')
					except Exception, e:
						rsp = rsp.decode('gbk')
					r = json.loads(rsp)
					if r["Protocol"] == '6003' or r["Protocol"] == '6004' or r["Protocol"] == '6005' :
						if r['ErrCode'] <> '0' :
							print r['ErrCode'],r['ErrDesc']
					elif r['ErrCode'] == '0' :
						self.lock.release()
						return r["RetData"]
					else:
						print r['ErrCode'],r['ErrDesc']   			 			
		except Exception, e:
			exstr = traceback.format_exc()
			print exstr
		self.lock.release()   	
		return None
	def buy(self,market,stockcode,price,amount,envtype = 0):
		print "buy"
		if market == "hk":
			req = {'Cookie':'123456',\
				'OrderSide':'0',\
				'OrderTypeHK':'0',\
				'Price':str(int(math.floor(price * 1000))),\
				'Qty': str(amount),\
				'StockCode':str(stockcode),\
				'EnvType': str(envtype)}
		return self.__socket_call(6003,req)
	def sell(self,market,stockcode,price,amount,envtype = 0):
		print "sell"
		if market == "hk":
			req = {'Cookie':'123456',\
				'OrderSide':'1',\
				'OrderTypeHK':'0',\
				'Price':str(int(math.floor(price * 1000))),\
				'Qty': str(amount),\
				'StockCode':str(stockcode),\
				'EnvType': str(envtype)}
		return self.__socket_call(6003,req)
	def get_depth(self,market,stockcode,depth = 5):
		  req = {'Market':str(self.__get_market_code(market)),\
		  		'StockCode':str(stockcode),\
		  		'GetGearNum':str(depth)\
		  	}
		  data = self.__socket_call(1002,req)
		  if(data is None):
			return None
		  for i in data['GearArr']:
		  	i['BuyPrice'] = round(float(i['BuyPrice']) / 1000,3)
		  	i['BuyVol'] = int(i['BuyVol'])
		  	i['SellPrice'] = round(float(i['SellPrice']) / 1000,3)
		  	i['SellVol'] = int(i['SellVol'])
		  return data['GearArr']
	def get_ticker(self,market,stockcode):
		  req = {'Market':str(self.__get_market_code(market)),\
		  		'StockCode':str(stockcode),\
		  	}
		  data = self.__socket_call(1001,req)
		  if(data is None):
			return None
		  for i in ('Cur','High','Low','Close','Open','LastClose','Turnover'):
		  	data[i] = round(float(data[i]) / 1000,3)
		  data['Vol'] = int(data['Vol'])
		  return data
	def cancel(self,market,order_id,envtype = 0):
		  if market == 'hk':
		  	req = {'Cookie':'123456',\
		  		'OrderID':str(order_id),\
		  		'SetOrderStatusHK':'0',\
		  		'EnvType':str(envtype),\
		  	}
		  data = self.__socket_call(6004,req)
		  return data
	def modify(self,market,order_id,price,amount,envtype = 0):
		  if market == 'hk':
		  	req = {'Cookie':'12345',\
		  		'OrderID':str(order_id),\
		  		'Price':str(int(math.floor(price * 1000))),\
		  		'Qty': str(amount),\
		  		'EnvType':str(envtype),\
		  	}
		  data = self.__socket_call(6005,req)
		  return data		
if __name__ == '__main__':
	futu = Futu()
	print futu.get_depth("hk","00700")
	#r =  futu.buy("hk","00700",148.1,100,1)
	#print r,int(r['OrderID'])
	#print futu.sell("hk","00700",140.1,100,1)
	#print futu.get_ticker("hk","00700")
	#print futu.modify("hk",int(r['OrderID']),148.0,100,1)
	#print futu.cancel("hk",int(r['OrderID']),1)
	 
	
		