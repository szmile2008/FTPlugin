#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
'''
Created on 2015年11月5日
#脚本说明: !!!
#
#当腾讯的价格到达145的时候，以卖一的价格，买入100股!
#
@author: futu
'''

import futu_comom_api
import time

buy_condition_price = 145 * 1000
stock_code = "00700"
buy_num = 100

socket_to_futu_api = futu_comom_api.connect_to_futunn_api("localhost", 11111)
if(socket_to_futu_api is not None):
	while True:
		#获取当前价格
		dic_base_price = futu_comom_api.get_stock_base_price(socket_to_futu_api, stock_code)
		if(dic_base_price is None):
			print("获取当前价格错误")
			time.sleep(5)
			continue
		cur_price = dic_base_price["Cur"]

		#当价格到达条件
		if(int(cur_price) >= int(buy_condition_price)):
			print(("股票:%s当前价格为%0.3f,超过%0.3f") % (stock_code, (float(cur_price))/1000, (float(buy_condition_price))/1000))
			#获取卖一价格
			dic_gear_info_arr = futu_comom_api.get_stock_gear(socket_to_futu_api, stock_code, 1)
			if(dic_gear_info_arr is None):
				print("获取买卖档口错误")
				break
			sell_price_one = dic_gear_info_arr[0]["SellPrice"]
			
			#买入
			print(("以卖一价格%0.3f买入%d股") % ((float(sell_price_one))/1000, int(buy_num)))
			futu_comom_api.place_order(socket_to_futu_api, 1, 0, 0, sell_price_one, buy_num, stock_code)
			break
		#轮询
		time.sleep(1)
	futu_comom_api.disconnect(socket_to_futu_api)
raw_input("按任意键退出")
