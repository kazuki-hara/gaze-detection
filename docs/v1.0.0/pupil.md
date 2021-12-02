# 瞳孔検出アルゴリズム

```flow
s=>start: 開始
e=>end: 終了

input=>inputoutput: Foveから得られた目の画像
circles=>inputoutput: 瞳孔領域の探索開始点
output1=>inputoutput: 瞳孔位置
output2=>inputoutput: 検出エラー

preprocess=>operation: 画像の前処理
hough=>operation: ハフ変換



s->input->preprocess->hough->circles->e
```
