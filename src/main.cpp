//
// JPY レート取得
//
// The MIT License (MIT)
//
// Copyright (c) <2023> chromabox <chromarockjp@gmail.com>
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
#include <vector>
#include <string>

#include "curl_easy.h"
#include "curl_utility.h"
#include "picojson.h"

using curl::curl_ios;
using curl::curl_easy;
using curl::curl_easy_exception;
using curl::curl_utility;
using curl::curlcpp_traceback;

using std::string;
using std::ostringstream;
using std::string_view;
using namespace std::literals::string_view_literals;




// バージョン
static std::string_view THIS_VERSION = "0.0.2"sv;

// レート取得用URL。GMOコインのPublic APIを使う
static std::string_view RATE_URL = "https://forex-api.coin.z.com/public/v1/ticker"sv;
// JSONの解析をする
static bool parseJson(const string &src,picojson::array &jarray)
{
	picojson::value jsonval;	
	string json_err;
	
	picojson::parse(jsonval,src.begin(),src.end(),&json_err);
	if(!json_err.empty()){
		std::cout << "[JSON] parse err!!! " << std::endl;
		std::cout << json_err << std::endl;
		std::cout << src << std::endl;
		return false;
	}
	if(!jsonval.is<picojson::object>()){
		std::cout << "[JSON] is not object... " << std::endl;
		return false;
	}
	picojson::object jobj = jsonval.get<picojson::object>();
	if(! jobj["data"].is<picojson::array>()){
		std::cout << "[JSON] is not array... " << std::endl;
		return false;
	}
	jarray = jobj["data"].get<picojson::array>();
	return true;
}

// USD/JPYの為替レートを取得
static bool readRate(picojson::object &rateobj)
{
	ostringstream stream;
	curl_ios<ostringstream> ios(stream);
	curl_easy peer(ios);

	peer.add<CURLOPT_URL>(RATE_URL.data());
	peer.add<CURLOPT_FOLLOWLOCATION>(1L);
	peer.add<CURLOPT_HTTPGET>(1L);
	// HTTPS取得
	try {
		peer.perform();
	} catch (curl_easy_exception &error) {
		// エラー内容表示
		std::cerr<<error.what()<<std::endl;
		return false;
	}
	// 取得できたのでHTTPコードが200かどうかを確認
	auto httpcode = peer.get_info<CURLINFO_RESPONSE_CODE>();
	if(httpcode.get() != 200){
		std::cout << "HTTP code error " << httpcode.get() << std::endl;
		return false;
	}
	
	// JSONでやってくるので解析
	picojson::array jarray;
	if(! parseJson(stream.str(),jarray)){
		return false;
	}
	
	// テーブルなので、その中からUSD_JPYを探す
	for(const auto &rate: jarray){
		if(! rate.is<picojson::object>()) continue;
		
		rateobj = rate.get<picojson::object>();
		if(rateobj["symbol"].to_str() == "USD_JPY"){
			return true;
		} 
	}
	std::cout << "ERROR: symbol USD_JPY not found." << std::endl;
	return false;
}


int main(int argc,char *argv[])
{
	picojson::object rate;

	for(;;){
		if(readRate(rate)){
			// レート表示。ask=買い bid=売り
			std::cout << "bid:" << rate["bid"].to_str() << " ask:" << rate["ask"].to_str() << std::endl;
		}
		sleep(60);					// 60秒間待ち
	}
	
	return 0;
}


