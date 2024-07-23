# UnknownLiDARMini_M5StackCore2

アリエクのジャンク中華ミニ LiDAR を M5Stack Core2 (ESP32) , PlatformIO で使うコードです。

製品リンク（ https://ja.aliexpress.com/item/1005007119470172.html ）

![image](https://github.com/devemin/UnknownLiDARMini_M5StackCore2/assets/52738228/1af608ec-e58e-4f54-b4f7-3d0dd5b4d14e)


どうやらこの LiDAR は、Camsense X1 と似たパケットデータとなっていました。


@yishii さんのCamsense X1 用コードが基本となっています。感謝！


# 環境

M5Stack Core2

PlatformIO

# ピンアサイン

![image](https://github.com/devemin/UnknownLiDARMini_M5StackCore2/assets/52738228/1e25e9b1-362c-4d86-8162-2a21ba2d3334)

このコードでは、

| (Core2) | (LiDAR) |
----|---- 
| GPIO 32 pin | TX pin |
| 5V | 5V |
| GND | GND |

としました。

(ケーブルの色が紛らわしいので間違えないよう注意！)

# パケット例

https://x.com/devemin/status/1809545109446513061

https://x.com/devemin/status/1809579265714975058

# デモ

https://x.com/devemin/status/1809596244073054370



# Camsense X1 コードからの差異

・ヘッダ (4byte) 変更

・8個の深度データ　→　16個に

・0x3FFF でマスクして上位の何かのビット 2bit 除去 (error & flag ?)


# 試行錯誤のスレッドリンク

https://x.com/devemin/status/1807955915838218345

# LICENSE

This code is from his code.

Thanks!!!  :-)

https://github.com/yishii/LiDAR_Camsense_X1_M5Stack
