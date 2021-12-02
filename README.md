# hara_fove

Foveから瞳孔検出→視線推定を行い，ジェスチャー検出とカメラ制御を行うプログラム


## 使い方

1. Foveの準備

    ```sh
    fove

    # aliasが登録されていない場合は以下
    ./Fove/FoveLinuxBuild/FoveServiceApp
    ```

2. 雲台の準備
    2-1. 雲台の電源を入れる
    2-2. 新規ターミナルで雲台との通信を開始

    ```sh
    telnet cammount 4000
    ```

    2-3. 雲台のキャリブレーション

    ```sh
    # ###PAN-TILT CONTROLLERみたいなのが表示されたら以下を入力
    rp
    ```

3. 新規ターミナルでソースコードを開始

    ```sh
    sh ./start.sh
    ```

## ディレクトリ構成

### [src](.src)

Foveと雲台を動かすためのソースコードはここ

#### [camera](.src/camera)

`socket.cpp`で雲台を動かす．
`capture.cpp`でステレオカメラから画像を取得．

#### [fove](.src/fove)

`display.cpp`でFove内のディスプレイに画像を表示．
`camera.cpp`でFove内のカメラから目の画像を取得．

#### [gaze](.src/gaze)

`gaze.cpp`で目の画像から瞳孔位置検出．

#### [utils](.src/utils)

Foveと雲台に直接関係ないソースコードを置く．
ディレクトリ内のファイル検索など

### [data](.data)

実験結果の分析用プログラム．言語はPython

#### `gaze.py`

検出した瞳孔位置のx-tグラフ，y-tグラフを作成

## 動かないとき

1. ライブラリが足りない場合は以下をインストール

    ```sh
    sudo apt-get install gtkmm-2.4-dev libopencv-dev python3-opencv
    ```

2. カメラ関係でプログラムがエラーを吐く場合

   ソースコードでは，ステレオカメラとFove内のカメラ共にOpenCVで画像を取得している．OpenCVではカメラの判別にデバイス名を利用できないため，PCへの接続順で割り振られるIDでカメラを判別している．ステレオカメラのIDを0，Fove内のカメラのIDを1としているので，うまく動かないときは一度FoveとステレオカメラをPCから外して，ステレオカメラ→Foveの順でPCに接続してみてください．

3. その他

    ファイルの出力先のディレクトリは存在するか？

## 記録

2021/12/2 Fove内のカメラから瞳孔検出する部分が一旦実装完了
