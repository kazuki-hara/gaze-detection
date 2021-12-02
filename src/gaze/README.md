# gaze

目の画像から瞳孔検出をするプログラム

## gaze.cpp

<a id="remove_blanc_line"></a>

- `void remove_blank_line(cv::Mat & original_image)`

    Foveから目の画像を取得したときに現れる両目の間の白線を取り除く関数．

    | 入力 | 出力 |
    | --------- | ----------- |
    | Foveから得られる目の生画像 (分割前の両目の画像) | 無し |

<a id="remove_outside_area"></a>

- `void remove_outside_area(cv::Mat& binarized_image)`
  
    Foveから取得した目の画像特有の周囲の黒い枠を取り除く関数．瞳孔検出時に画素値が白の場合は検出エラーとしたいため，極力黒い面積を取り除くために使用．
    アルゴリズムとしては，画像の各行をx方向に探索．左端と右端から黒い画素が続く限りは，その画素をFoveの枠として扱い，画素値を255に変更．

    | 入力 | 出力 |
    | --------- | ----------- |
    | 二値化後の片目の画像 | 無し |

- `cv::Mat remove_outside_area_v2(cv::Mat binarized_image)`
  
    Foveから取得した目の画像特有の周囲の黒い枠を取り除く関数 (バージョン2)．アルゴリズムとしては，まずOpenCVの領域判定API `cv::findContours` を用いて二値化画像を領域分割したときのエッジを取得．領域の面積計算API `cv::contourArea` で各領域の面積を計算し，面積が閾値以内の場合は瞳孔の可能性がある領域として画素を黒にする．現状は領域を取り除きすぎて(?)，ハフ変換による円検出の精度が下がったため使用していない．

    | 入力 | 出力 |
    | --------- | ----------- |
    | 二値化後の片目の画像 | 瞳孔の可能性がある領域のみを黒くした二値化画像 |

- `cv::Mat draw_hough_circles(cv::Mat original_image, std::vector<cv::Vec3f> left_circles, std::vector<cv::Vec3f> right_circles)`

    ハフ変換によって検出した円を，Foveから取得した目の画像に載せた画像を取得する関数．ハフ変換による円検出の精度や，どこら辺を誤検出しているかを目視で確認するために作成．

    | 入力 | 出力 |
    | --------- | ----------- |
    | 1. Foveから得られる目の生画像 (分割前の両目の画像) <br> 2. 左目画像から検出した円のvector <br> 3. 右目画像から検出した円のvector| 両目の生画像にハフ変換によって検出した円を赤く表示させた画像 |

- `EyeInfoGetterV2`

    Foveから得られる目の画像から瞳孔位置を検出するクラス．V1は精度がゴミなので，[old.cpp](./old.cpp)に移動しました．
    基本的には，

    ```cpp
    // コンストラクタ
    EyeInfoGetterV2 eye_info_getter;

    // 瞳孔検出
    std::tuple<double, double, double, double> pupil_pos = eye_info_getter.detect_pupil_center(input_image);

    // (lx, ly, rx, ry) = (左目x座標，左目y座標, 右目x座標, 右目y座標)
    double lx = std::get<0>(pupil_pos);
    double ly = std::get<1>(pupil_pos);
    double rx = std::get<2>(pupil_pos);
    double ry = std::get<3>(pupil_pos);
    ```

    で使用可能．
    以下にメンバ関数の説明を述べる．

    <a id="convert_image"></a>

  - `std::tuple<cv::Mat, cv::Mat> EyeInfoGetterV2::convert_image(cv::Mat original_image)`

    Foveから得られた目の生画像を，左目画像と右目画像に分割する関数．目の生画像には，中央部に謎の白線が入っているので，[`remove_blank_line(original_image)`](#remove_blank_line)で白線を除去している．その後，画像をグレースケール変換して，中央で左目画像と右目画像に分割．

    | 入力 | 出力 |
    | --------- | ----------- |
    | Foveから得られる目の生画像 (分割前の両目の画像) | 左目画像と右目画像のタプル |

    <a id="preprocess"></a>

  - `void EyeInfoGetterV2::preprocess(cv::Mat& eye_image)`

    左目画像・右目画像に対して前処理を行う．
    以下は前処理の内容

    1. 二値化

    2. Fove特有の黒い外枠を除去 ([`remove_outside_area(eye_image)`](#remove_outside_area))

    3. 平滑化

    4. Cannyのエッジ処理

    5. 平滑化

    | 入力 | 出力 |
    | --------- | ----------- |
    | 片目の画像 | 無し |

    <a id="detect_pupil_circle"></a>

  - `std::vector<cv::Vec3f> EyeInfoGetterV2::detect_pupil_circle(cv::Mat eye_image)`

    前処理した片目画像から円を検出．円検出にはハフ変換API `cv::HoughCircles` を利用．最小半径，最大半径が設定可能で，明るさや時間経過による瞳孔面積の変化に対応．検出した円は `cv::Vec3f` 形式で(ピクセルのx座標，ピクセルのy座標，半径)のデータが得られる．1枚の画像から複数の円を検出するので，戻り値はvector形式．

    | 入力 | 出力 |
    | --------- | ----------- |
    | 前処理後の片目画像 | 検出した円のvector |

    <a id="cal_center_of_gravity"></a>

  - `std::tuple<double, double> EyeInfoGetterV2::cal_center_of_gravity(cv::Mat binarized_image, std::vector<cv::Vec3f> circles)`

    二値化画像とハフ変換によって検出した円から瞳孔の位置を特定する関数．

    | 入力 | 出力 |
    | --------- | ----------- |
    | 1. 二値化した片目画像 <br> 2. ハフ変換によって検出した円のvector | 瞳孔位置のx座標とy座標のタプル |

  - `std::tuple<double, double, double, double> EyeInfoGetterV2::detect_pupil_center(cv::Mat original_image)`
  
    Foveから得られた目の生画像から各目の瞳孔位置を検出する一連の流れをまとめた関数．基本的にこれを使えばOK.
    処理の主な流れとしては
    1. 目の生画像からグレースケール変換した左目画像・右目画像を取得 ([`convert_image(original_image)`](#convert_image))

    2. 片目画像に対して，前処理を行う．([`preprocess(eye_image)`](#preprocess))

    3. ハフ変換を利用して，前処理後の片目画像から円を検出 ([`detect_pupil_circle(eye_image)`](#detect_pupil_circle))

    4. 片目画像とハフ変換によって検出した円から瞳孔位置を特定 ([`cal_center_of_gravity(binarized_image, circles)`](#cal_center_of_gravity))

    5. 2~4を左目画像，右目画像に対してそれぞれ行う．

    | 入力 | 出力 |
    | --------- | ----------- |
    | Foveから得られる目の生画像 (分割前の両目の画像) | 瞳孔位置のx座標とy座標のタプル |

  - `cv::Mat EyeInfoGetterV2::draw_pupil_center(cv::Mat original_image, std::tuple<double, double, double, double> pupil_pos)`
  
    Foveから得られた目の生画像に，検出した瞳孔位置を表示させる関数．うまく検出できているか，誤検出している場合はどこらへんを瞳孔と勘違いしているのか等を確認するために使用．

    | 入力 | 出力 |
    | --------- | ----------- |
    | 1. Foveから得られる目の生画像 (分割前の両目の画像) <br> 2. 瞳孔位置のx座標とy座標のタプル | 検出した瞳孔位置をマークした画像 |