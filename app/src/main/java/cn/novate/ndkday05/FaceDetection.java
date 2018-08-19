package cn.novate.ndkday05;

import android.graphics.Bitmap;

/**
 * Email: 2185134304@qq.com
 * Created by Novate 2018/8/19 10:10
 * Version 1.0
 * Params:
 * Description:    人脸识别
*/

public class FaceDetection {


    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");  // native-lib：这个名字可以随便修改
    }

    /**
     * 检测人脸，并保存人脸信息
     */
    public native int faceDetectionSaveInfo(Bitmap mFaceBitmap) ;

    /**
     * 加载人脸识别 分类器文件
     */
    public native void loadCascade(String filePath) ;
}
