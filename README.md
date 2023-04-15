jpyrate
=======

為替レートをC++(use curl library)で簡単に取得するサンプルです  
curlcppのサンプルでもあります  
  
[![CodeFactor](https://www.codefactor.io/repository/github/chromabox/jpyrate/badge)](https://www.codefactor.io/repository/github/chromabox/jpyrate)
  
  
使い方
===============

コンパイル
--------

jpyrateは以下のパッケージを使用しています。
* g++
* STL(libstdc++)
* libcurl-dev (opensslでもnssでも何でもいい)
* make
* cmake (3．13以降。curlcppのビルドに必要)

ので、あらかじめインストールしてください。
vscodeがあると更に良いです  

Ubuntuの場合は次のようにしたら良いかもしれません。
````
$ sudo apt-get install g++ libstdc++6 libcurl4-openssl-dev cmake make
````
あとは
````
$ make
````
でbuildにjpyrateが作成されますので
````
$ cd build
$ ./jpyrate
````
としてください  
サンプルではUSD/JPY（ドル円）のレートを１分おきに取得するようにしています。  
  
````
$ ./jpyrate
bid:120.35 ask:120.36  
bid:120.32 ask:120.33  
bid:120.33 ask:120.34  
bid:120.32 ask:120.33  
bid:120.32 ask:120.33  
````

売り値：買い値 の順番で表示します。
Ctrl+Cで終わります。

注意
===============

これは外為オンラインのデータを使用しています。
正規の方法では無いので、Sleepの間隔を短くして頻繁にアクセスするのは控えましょう。

個人で細々と使う分にはまぁ良いと思いますが、データを商用利用するとか、そういうのはまずいと思います。

参考文献
===============

Googleの為替レートAPIが利用できなくなっていた件
https://8wired.jp/blog/1121


謝辞
===============
* picojsonのkazuho氏 (https://github.com/kazuho/picojson)  
JSONの解析に使用しています。picojsonはSTLの親和性高くて便利です  
* curlcppのJosephP91氏 (https://github.com/JosephP91/curlcpp)  
httpsの取得に使用しています。これはcurlライブラリのc++ラッパであり扱いやすくなります。