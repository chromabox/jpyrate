//
// JPY レート取得
//
// The MIT License (MIT)
//
// Copyright (c) <2014> chromabox <chromarockjp@gmail.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <getopt.h>
#include <unistd.h>

#include "http/httpcurl.hpp"
#include "include/picojson.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

using namespace std;

// バージョン
static const string THIS_VERSION	= "0.0.1";

// レート取得用URL。外為オンラインのを使う
static const std::string	RATE_URL	= "http://www.gaitameonline.com/rateaj/getrate";


// JSONの解析をする
bool parseJson(const string &src,picojson::array &jarray)
{
	picojson::value jsonval;	
	string json_err;
	
	picojson::parse(jsonval,src.begin(),src.end(),&json_err);
	if(!json_err.empty()){
		cout << "[JSON] parse err!!! " << endl;
		cout << json_err << endl;
		cout << src << endl;
		return false;
	}
	if(!jsonval.is<picojson::object>()){
		cout << "[JSON] is not object... " << endl;
		return false;
	}
	picojson::object jobj = jsonval.get<picojson::object>();
	if(! jobj["quotes"].is<picojson::array>()){
		cout << "[JSON] is not array... " << endl;
		return false;
	}
	jarray = jobj["quotes"].get<picojson::array>();
	return true;
}

// USD/JPYの為替レートを取得
bool readRate(picojson::object &rateobj)
{
	HTTPCurl peer;
	picojson::array jarray;
	
	if(!peer.getRequest(RATE_URL,"")){
		cout << "request error" << endl;
		return false;
	}
	unsigned long httpcode = peer.getLastResponceCode();
	if(httpcode != 200){
		printf("HTTP code Error %lu\n",httpcode);
		return false;
	}
	// JSONでやってくるので解析
	string responce = peer.getResponceString();
	if(! parseJson(responce,jarray)){
		return false;
	}
	
	// テーブルなので、その中からUSDJPYを探す
	picojson::array::reverse_iterator it;
	
	for(it=jarray.rbegin();it!=jarray.rend();it++){
		if(! it->is<picojson::object>()) continue;
		
		rateobj = it->get<picojson::object>();
		if(rateobj["currencyPairCode"].to_str() == "USDJPY") break;
	}
	return true;
}


int main(int argc,char *argv[])
{
	picojson::object rate;
	for(;;){
		readRate(rate);
		
		// レート表示。ask=買い bid=売り
		cout << "bid:" << rate["bid"].to_str() << " ask:" << rate["ask"].to_str() << endl;
		sleep(60);					// 60秒間待ち
	}
	
	return 0;
}


