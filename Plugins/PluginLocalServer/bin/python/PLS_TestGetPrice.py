#!/usr/bin/python2.6  
# -*- coding: utf-8 -*-  
'''
Created on 2015年9月9日

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

#发送请求: 港股00700的当前报价 
def fun_GetQuotePrice(args=[]):
    print "test get price begin..."
    s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.connect((host,port))
    req = {'Protocol':'1001', 'ReqParam':{'Market':'1','StockCode':'00700'},'Version':'1'}
    str = json.dumps(req) + "\r\n"
    s.send(str) 
    rsp = s.recv(4096)
    str = rsp[:rsp.index("\r\n")]
    print str
    decode = json.loads(str)
    err = decode["ErrCode"]
    print "GetPrice Err=%s"%err
    if err == "0":
        cp = decode["RetData"]["Close"]
        lc = decode["RetData"]["LastClose"]
        hi = decode["RetData"]["High"]
        lo = decode["RetData"]["Low"]
        print "cp=%s, lc=%s, hi=%s, lo=%s"%(cp, lc, hi, lo)
    s.close()
    print "test get price end!"
    print ""
    t1 = CountDownExec(2, fun_GetQuotePrice)
    t1.start()

t1 = CountDownExec(2, fun_GetQuotePrice)
t1.start()


    



 




