# src

Foveと雲台を動かすためのプログラム

## ディレクトリ構成

### [camera](./camera)

`socket.cpp`で雲台を動かす．
`capture.cpp`でステレオカメラから画像を取得．

### [fove](./fove)

`display.cpp`でFove内のディスプレイに画像を表示．
`camera.cpp`でFove内のカメラから目の画像を取得．

### [gaze](./gaze)

`gaze.cpp`で目の画像から瞳孔位置検出．

### [utils](./utils)

Foveと雲台に直接関係ないソースコードを置く．
ディレクトリ内のファイル検索など

### `main.cpp`

main関数はここ．雲台制御用，ステレオカメラから画像を取得用，Foveから視線検出用，時刻同期用のスレッドを立てる．
