#!/usr/bin/python2.6  
# -*- coding: utf-8 -*-  
'''
Created on 2015年9月9日
#脚本说明: !!!
#
#仿真交易环境下，向svr提交一个增强限价单: 买入港股00700 /100股/ 价格150.000  
#并回显定单的状态， 全部成交或交易异常,程序退出
#
@author: futu
'''

import socket
import json
import string
import sys
import threading
import time

################################################################################################## 
#timer定时器的一个实现
class Timer(threading.Thread):
    """
    very simple but useless timer.
    """
    def __init__(self, seconds):
        self.runTime = seconds
        threading.Thread.__init__(self)
    def run(self):
        time.sleep(self.runTime)
        print "Buzzzz!! Time's up!"
class CountDownTimer(Timer):
    """
    a timer that can counts down the seconds.
    """
    def run(self):
        counter = self.runTime
        for sec in range(self.runTime):
            time.sleep(1.0)
            counter -= 1
class CountDownExec(CountDownTimer):
    """
    a timer that execute an action at the end of the timer run.
    """
    def __init__(self, seconds, action, args=[]):
        self.args = args
        self.action = action
        CountDownTimer.__init__(self, seconds)
    def run(self):
        CountDownTimer.run(self)
        self.action(self.args)
        

################################################################################################## 

#futnn plubin会开启本地监听服务端 
# 请求及发送数据都是jason格式, 具体详见插件的协议文档 

host="localhost"
port=11111

global g_buy, g_lid, g_sid, g_end

#是否发送了请求
g_buy = 0
#订单本地id 
g_lid = 0
#订单svr id 
g_sid = 0
#是否结束了
g_end = 0

#socket
g_s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
g_s.connect((host,port))


def fun_TestTrade(args=[]):
    print "enter fun testTrade ..."
    req1 = {'Protocol':'6003','ReqParam':{'EnvType':'1', 'Cookie':'8888','OrderSide':'0','OrderType':'0','Price':'150000','Qty':'100','StockCode':'00700'},'Version':'1'}
    global g_buy, g_lid, g_sid, g_end
    if g_buy == 0:
        str = json.dumps(req1) + "\r\n"
        g_s.send(str)
        g_buy = 1
    print "Wait trade order respons ..." 
    rsp = g_s.recv(4096)
    arstr = rsp.split("\r\n") 
    for str in arstr:
        if str != "":
            print str
            decode = json.loads(str)
            pro = decode["Protocol"]
            #解析发送请求
            if pro == "6003":
                cookie = decode["RetData"]["Cookie"]
                if cookie == "8888":
                    print "My Req Return"
                    g_lid = decode["RetData"]["LocalID"]
                    err = decode["ErrCode"]
                    if err != "0":
                        print "err=%s %s"%(err, decode["ErrDesc"])
                        g_end = 1
                        
            elif pro =="6001":
                lid = decode["RetData"]["LocalID"]
                if lid == g_lid:
                    g_sid = decode["RetData"]["OrderID"]
                    dtqty = decode["RetData"]["DealtQty"]
                    qty = decode["RetData"]["Qty"]
                    print "order status changed id=%s dealtQuty=%s"%(g_sid, dtqty)
                    if dtqty == qty:
                        g_end = 1
                        print "order finish!!" 
                print str
            elif pro == "6002":
                sid = decode["RetData"]["OrderID"]
                if g_sid == sid:
                    print "order Err:%s"%(decode["RetData"]["OrderErrNotifyHK"])
                    g_end = 1
                print str
                                           
    print ""
    if g_end == 0:
        t1 = CountDownExec(2, fun_TestTrade)
        t1.start()
    else:
        g_s.close()
        print "test trade all over"
    print "leave fun testTrade ...\n"
    
t1 = CountDownExec(2, fun_TestTrade)
t1.start()


    



 




