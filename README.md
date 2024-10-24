## 概要
これは以下のリンクの商品を使ってサムターンの状況からドアの鍵が開いているのか、閉まっているのかをDiscordにWebhookで送信するプログラムです。<br>
[OSOYOO Arduino用 Raspberry Pi 5 対応 IR赤外線障害物回避 センサモジュール](https://www.amazon.co.jp/gp/product/B07CG5L5NQ/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1)　<br>
以前はMicroPythonで書いていましたが、WiFi接続が不安定なのとネットワークにつながっているかがわからず状態を送信できなかったことが多かったので、C++で書き直しました。

## ESP32の環境
ESP32のファームウェアはデフォルトの状態です。

## VSCodeの環境構築
VSCodeの環境構築は以下のリンクを参考にしてください。(私が書いたページです)<br>
https://brawny-spy-652.notion.site/ESP32-10d3fa32da8340a6bfeb0e37e1e31679?pvs=4
