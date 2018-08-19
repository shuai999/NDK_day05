#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>

#include <android/log.h>


// log日志：有需要可以直接拷贝即可和上边的 #include <android/log.h>
#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

// 需要声明：使用命名空间 :: , 为的是可能需要在下边代码使用 ::
using namespace cv ;

extern "C"{

// 对下边的faceDetectionSaveInfo方法进行声明
JNIEXPORT jint JNICALL
Java_cn_novate_ndkday05_FaceDetection_faceDetectionSaveInfo(JNIEnv *env, jobject instance,
                                                            jobject mFaceBitmap) ;

// 对方法进行声明：bitmap - 转为 Mat矩阵：&mat引用
void bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap) ;

// 声明一下：mat 转成 Bitmap
void mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap) ;

// 声明一下：加载人脸分类器文件
JNIEXPORT void JNICALL
Java_cn_novate_ndkday05_FaceDetection_loadCascade(JNIEnv *env, jobject instance,
                                                  jstring filePath_) ;
}

// 下边的代码可以注释，没有用，这个是native-lib.cpp中默认创建的
/*JNIEXPORT jstring JNICALL
Java_cn_novate_ndkday05_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject *//* this *//*) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}*/


void bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap) {
    // Mat 源码中有个 type  -> Mat(int rows, int cols, int type);
    // CV_8UC4 刚好对上我们Bitmap中 ARGB_8888,
    // CV_8UC2 刚好对上我们Bitmap中 RGB_565

    // 1. 指针：获取bitmap的信息
    AndroidBitmapInfo info ;
    // 像素
    void* pixels ;
    AndroidBitmap_getInfo(env , bitmap , &info) ; // 这里给地址 &info 就可以


    /*********** 操作Bitmap套路 ************/
    // 1. 锁定 Bitmap 画布
    AndroidBitmap_lockPixels(env , bitmap , &pixels) ;
    // 指定 mat 的宽和高和type BGRA
    mat.create(info.height , info.width , CV_8UC4) ;

    if(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888){
        // 8888对应的Mat 应该是 CV_8UC4
        // 创建临时矩阵
        Mat temp(info.height , info.width , CV_8UC4 , pixels) ;

        // 需要把临时数据temp 复制到 mat矩阵里边来
        temp.copyTo(mat) ;


    }else if(info.format == ANDROID_BITMAP_FORMAT_RGB_565){
        // 对应的Mat 应该是 CV_8UC2
        // 创建临时矩阵
        Mat temp(info.height , info.width , CV_8UC2 , pixels) ;


        // mat 是 CV_8UC4，把CV_8UC2的数据要放到 CV_8UC4中
        // 需要把临时数据temp 复制到 mat矩阵里边来
        cvtColor(temp , mat , COLOR_BGR5652BGRA) ;
    }


    // 其他的 ANDROID_BITMAP_FORMAT_NONE、ANDROID_BITMAP_FORMAT_RGBA_4444 要自己去转

    // 解锁 Bitmap 画布
    AndroidBitmap_unlockPixels(env , bitmap) ;

}


// mat 转成 Bitmap：把 mat的数据复制到 bitmap里边去
void mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap) {
    // 1. 获取 bitmap 信息
    AndroidBitmapInfo info;
    void* pixels;
    AndroidBitmap_getInfo(env,bitmap,&info);

    // 锁定 Bitmap 画布
    AndroidBitmap_lockPixels(env,bitmap,&pixels);

    if(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888){  // C4
        Mat temp(info.height,info.width,CV_8UC4,pixels);

        // 在这里还需要if...else... 判断
        if(mat.type() == CV_8UC4){
            mat.copyTo(temp);
        }
        else if(mat.type() == CV_8UC2){
            cvtColor(mat,temp,COLOR_BGR5652BGRA);
        }
        else if(mat.type() == CV_8UC1){ // 灰度图的mat就是CV_8UC1
            cvtColor(mat,temp,COLOR_GRAY2BGRA);
        }


    } else if(info.format == ANDROID_BITMAP_FORMAT_RGB_565){  // C2
        Mat temp(info.height,info.width,CV_8UC2,pixels);


        if(mat.type() == CV_8UC4){
            cvtColor(mat,temp,COLOR_BGRA2BGR565);
        }
        else if(mat.type() == CV_8UC2){
            mat.copyTo(temp);

        }
        else if(mat.type() == CV_8UC1){// 灰度 mat
            cvtColor(mat,temp,COLOR_GRAY2BGR565);
        }
    }
    // 其他要自己去转

    // 解锁 Bitmap 画布
    AndroidBitmap_unlockPixels(env,bitmap);
}



CascadeClassifier cascadeClassifier ;
JNIEXPORT void JNICALL
Java_cn_novate_ndkday05_FaceDetection_loadCascade(JNIEnv *env, jobject instance,
                                                  jstring filePath_) {
    const char *filePath = env->GetStringUTFChars(filePath_, 0);

    // TODO
    cascadeClassifier.load(filePath) ;
    LOGE("加载分类器文件成功") ;

    env->ReleaseStringUTFChars(filePath_, filePath);
}




JNIEXPORT jint JNICALL
Java_cn_novate_ndkday05_FaceDetection_faceDetectionSaveInfo(JNIEnv *env, jobject instance,
                                                            jobject bitmap) {

    // TODO 检测人脸：在opencv里边，有一个很重要的类 Mat , opencv是c和c++写的，它只会处理 Mat，而android里边是Bitmap，所以需要：
    // 第一步：把Bitmap转为 opencv能操作的c++对象，因为它不能操作 Bitmap
    // Mat：是一个矩阵，在android中图片是Bitmap和Drawable，而在 c和c++中，图片就是一个Mat矩阵，而且会带有颜色通道
    Mat mat ;
    // 把Bitmap图片转为 Mat矩阵
    bitmap2Mat(env , mat , bitmap) ;

    // opencv处理灰度图，提高效率，一般所有的操作都会对其进行灰度处理
    Mat gray_mat ;  // 把mat 转为 灰度gray_mat
    cvtColor(mat , gray_mat , COLOR_BGRA2GRAY) ;


    // 再次处理 直方均衡补偿
    Mat equalize_mat ;
    // 直方图去处理
    equalizeHist(gray_mat , equalize_mat) ;

    // 识别人脸，可以直接用 彩色图去做，识别人脸还要去加载人脸分类器文件

    // 识别人脸的集合vector
    std::vector<Rect> faces;  // 1.1是默认参数，5表示最小重叠次数，经过5次判断正确的话才是人脸；人脸最小是多小，最大是多大
    cascadeClassifier.detectMultiScale(equalize_mat , faces , 1.1 , 5) ;

    // 打印log，需要用%d作为占位符，后边写对应大小
    LOGE("人脸个数：%d" , faces.size()) ;

    // 如果只有一张人脸
    if(faces.size() == 1){
        // 获取人脸的区域
        Rect faceRect = faces[0] ;
        // 保存人脸信息，其实保存的就是 mat，而本地的mat其实就是 png或者jpg的图片
        // 保存图片之后就可以上传至后台，这里不写
//        imwrite()

        // 在人脸部分画个图：就是把人脸放到一个框框
        rectangle(mat ,faceRect,Scalar(255 , 155 , 155) , 8) ;

        // 把 灰度的gray_mat 又重新放到 bitmap中，然后显示到手机之前显示ImageView的位置
        mat2Bitmap(env , mat , bitmap) ;

        // 保存人脸信息 Mat ，只去保存人脸框框中的一部分，
        Mat face_info_mat(equalize_mat,faceRect);
    }




    // TODO 保存人脸信息

    return 0 ;

}
