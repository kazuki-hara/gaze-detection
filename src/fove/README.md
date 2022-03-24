# fove

Fove内のディスプレイにキャリブレーション用画面やステレオカメラの映像を表示するプログラム．

## display.cpp

- `void put_2d_image_cv_ishikawa(GLdouble x, GLdouble y, GLdouble width, GLdouble height, GLdouble div)`
  
    画像を貼り付けるテクスチャに関する関数．石川さんが作ったもの

    | 入力 | 出力 |
    | --------- | ----------- |
    | 1. テクスチャの中心位置のx座標 <br> 
      2. | なし |

- `Display::Display(int h, int w)`

