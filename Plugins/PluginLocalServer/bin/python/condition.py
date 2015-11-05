#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
'''
Created on 2015年11月5日
#脚本说明: !!!
#
#当腾讯的价格到达145的时候，开始执行以下判断
# i.买入操作：
#   1.当卖一145的时候，买入100股
#   2.当卖一150的时候，买入100股
#   3.当卖一155的时候，买入100股
# ii.在整个过程中，监控最新价：
#   1.记录一个最高价，然后从最高价开始判断
#      a.跌2块的时候，以买一的价格，卖100股
#      b.跌3块的时候，以买一的价格，卖100股
#      c.跌4块的时候，以买一的价格，卖100股
#   2.当最新价跌破140的时候
#      a.以买一的价格全卖出。
#
@author: futu
'''


import futu_comom_api
import time

stock_code = "00700"
buy_num = 100
sell_num = 100

first_condition_price = 145 * 1000
buy_info_arr = [{"price":145 * 1000, "had_done":False}, {"price":150 * 1000, "had_done":False}, {"price":155 * 1000, "had_done":False}]
sell_info_arr =  [{"drop":2 * 1000, "had_done":False}, {"drop":3 * 1000, "had_done":False}, {"drop":4 * 1000, "had_done":False}]
all_sell_condition_price = 140 * 1000

hold_total = 0
highest_price = 0

socket_to_futu_api = futu_comom_api.connect_to_futunn_api("localhost", 11111)
if(socket_to_futu_api is not None):
	while True:
		#获取当前价格
		dic_base_price = futu_comom_api.get_stock_base_price(socket_to_futu_api, stock_code)
		if(dic_base_price is None):
			print("获取当前价格错误\r\n")
			continue
		cur_price = dic_base_price["Cur"]
		
		#记录最高当前价
		highest_price = max(int(cur_price), int(highest_price))
		
		#获取买卖档口信息
		dic_gear_info_arr = futu_comom_api.get_stock_gear(socket_to_futu_api, stock_code, 1)
		if(dic_gear_info_arr is None):
			print("获取买卖档口错误\r\n")
			break
		
		#获取买一卖一
		buy_price_one = dic_gear_info_arr[0]["BuyPrice"]
		sell_price_one = dic_gear_info_arr[0]["SellPrice"]
		
		#当价格到达条件买卖前提条件
		if(int(cur_price) >= int(first_condition_price)):
			#判断买入条件
			for buy_info in buy_info_arr:
				#没有买过并买入到达条件
				if(bool(buy_info["had_done"]) is False and int(sell_price_one) >= int(buy_info["price"])):
					print ("股票:%s当前价格为%0.3f,超过%0.3f") % (stock_code, (float(cur_price)) / 1000, (float(first_condition_price)) / 1000 )
					print("并且当前卖一价格为%0.3f,超过%0.3f") % ((float(sell_price_one)) / 1000, (float(buy_info["price"])) / 1000)
					print(("以卖一价格%0.3f买入%d股") % ((float(sell_price_one))/1000, int(buy_num)))
					
					if(futu_comom_api.place_order(socket_to_futu_api, 1, 0, 0, sell_price_one, buy_num, stock_code)):
						buy_info["had_done"] = True
						hold_total = int(hold_total) + int(buy_num)
					print(("当前持有%d股\r\n\r\n") % hold_total)
			
			#判断卖出条件
			for sell_info in sell_info_arr:
				if(bool(sell_info["had_done"]) is False and int(highest_price) - int(cur_price) >= sell_info["drop"] and int(hold_total) > 0):
					print("股票:%s当前价格为%0.3f,超过%0.3f") % (stock_code, (float(cur_price)) / 1000, (float(first_condition_price)) / 1000 )
					print("并且当前价格距离当前价格最高超过%0.3f") % ((float(sell_info["drop"])) / 1000)
					print(("以买一价格%0.3f卖出%d股") % ((float(buy_price_one))/1000, int(cur_sell_num)))
					
					if(futu_comom_api.place_order(socket_to_futu_api, 1, 1, 0, buy_price_one, cur_sell_num, stock_code)):
						sell_info["had_done"] = True
						cur_sell_num = min(int(sell_num), int(hold_total))
						hold_total = int(hold_total) - int(cur_sell_num)
					print(("当前持有%d股\r\n") % hold_total)
		
			#判断是否全部条件单下完
			conditon_trade_all_done = True
			for buy_info in buy_info_arr:
				if(bool(buy_info["had_done"]) is False):
					conditon_trade_all_done = False
					break
			
			for sell_info in sell_info_arr:
				if(bool(sell_info["had_done"]) is False):
					conditon_trade_all_done = False
					break
			
			if(bool(conditon_trade_all_done) is True):
				break;
			
		#全部卖出
		if(int(cur_price) <= int(all_sell_condition_price) and int(hold_total) > 0):
			print(("股票:%s当前价格%0.3f到达全部卖出条件") % (stock_code, (float(cur_price)) / 1000))
			print(("以买一价格%0.3f全部卖出%d股\r\n") % ((float(buy_price_one))/1000, int(hold_total)))
			futu_comom_api.place_order(socket_to_futu_api, 1, 1, 0, buy_price_one, hold_total, stock_code)
			break
		
		#轮询
		time.sleep(1)
	futu_comom_api.disconnect(socket_to_futu_api)	

raw_input("按任意键退出")

