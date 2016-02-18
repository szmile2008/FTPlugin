#!/usr/bin/python2.6  
# -*- coding: utf-8 -*-  
'''
Created on 2015年8月26日

@author: futu
'''

import socket
import json
import string
import sys

#futnn plubin会开启本地监听服务端 
# 请求及发送数据都是jason格式, 具体详见插件的协议文档 
host="localhost"
port=11111

s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect((host,port))

print "test quote price 00700"
print "-------------------------------------"
#发送报价请求 
req = {'Protocol':'1001', 'ReqParam':{'Market':'1','StockCode':'00700'},'Version':'1'}
str = json.dumps(req) + "\n"
print str
s.send(str) 
rsp = ""
while True:
    buf = s.recv(1024)
    rsp = rsp + buf
    #找到"\n"就认为结束了
    try:
        rsp.index('\n')
        break;
    except Exception, e:
        print "recving..."
print rsp

print "test trade buy 0700\n"
print "-------------------------------------"
#发送交易请求
req = {'Protocol':'6003','ReqParam':{'Cookie':'123456','OrderSide':'0','OrderType':'1','Price':'150','Qty':'22200','StockCode':'00700'},'Version':'1'}
str = json.dumps(req) + "\n"
print str
s.send(str) 
rsp = ""
while True:
    buf = s.recv(1024)
    rsp = rsp + buf
    #找到"\n"就认为结束了
    try:
        rsp.index('\n')
        break;
    except Exception, e:
        print "recving..."
print rsp
print "-----------------over----------------"

s.close()




