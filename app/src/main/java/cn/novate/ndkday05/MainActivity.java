package cn.novate.ndkday05;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Email: 2185134304@qq.com
 * Created by Novate 2018/8/19 17:07
 * Version 1.0
 * Params:
 * Description:
*/

public class MainActivity extends AppCompatActivity {

    private ImageView mFaceImagae ;
    private Bitmap mFaceBitmap ;
    private FaceDetection mFaceDetection ;
    private File mCascadeFile;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        // 实际中可以把xml文件中的ImageView换成SurfaceView，打开相机把SurfaceView放上去，
        // 然后不断的获取SurfaceView的Bitmap，然后不断传递给native层

        // 这里为了方便，直接给 native层传递一张图片
        mFaceImagae = (ImageView) findViewById(R.id.face_image);
        mFaceBitmap = BitmapFactory.decodeResource(getResources() , R.drawable.face) ;
        mFaceImagae.setImageBitmap(mFaceBitmap);



//        copyCascadeFile() ;

        mFaceDetection = new FaceDetection() ;
//        mFaceDetection.loadCascade(mCascadeFile.getAbsolutePath());
    }



    private void copyCascadeFile() {
        try {
            // load cascade file from application resources
            InputStream is = getResources().openRawResource(R.raw.lbpcascade_frontalface);
            File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
            mCascadeFile = new File(cascadeDir, "lbpcascade_frontalface.xml");
            if(mCascadeFile.exists()) return;
            FileOutputStream os = new FileOutputStream(mCascadeFile);

            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) {
                os.write(buffer, 0, bytesRead);
            }
            is.close();
            os.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    /**
     * 点击人脸识别
     */
    public void faceDetection(View view) {
        // 识别人脸，保存人脸特征信息
        mFaceDetection.faceDetectionSaveInfo(mFaceBitmap) ;
        // 点击人脸识别后，图片就会变成一张灰度图
        mFaceImagae.setImageBitmap(mFaceBitmap);
    }
}
