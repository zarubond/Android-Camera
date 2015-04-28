package com.zarubond.lookingglass;

import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.graphics.SurfaceTexture;
import android.widget.FrameLayout;
import android.view.ViewGroup.LayoutParams;
import java.io.IOException;
import android.app.Activity;
import java.util.List;

public class CameraBackendSW implements Camera.PreviewCallback {

    private Camera camera;
    private static SurfaceView view;
    private int width, height;
	
    public CameraBackendSW(){
        width=0;
        height=0;
    }	
/*
    This method has to be called from Activity's onCreate method.
*/
    public static void setup(FrameLayout mainLayout, Activity activity) {

        SurfaceView camView = new SurfaceView(activity);
        view=camView;
        mainLayout.addView(camView, new LayoutParams(1, 1));
    }

    public void startPreview (int width, int height){
        camera = Camera.open();

        this.width=width;
        this.height=height;

        try
        {
            SurfaceHolder holder=view.getHolder();
            camera.setPreviewDisplay(holder);
        }
        catch (IOException e)
        {
            camera.release();
            return;
        }

        Camera.Parameters parameters = camera.getParameters();
/*
        List<int[]> fpslist = parameters.getSupportedPreviewFpsRange();
        //Log.d("camera", "size= " + fpslist.size());
        for (int i=0;i < fpslist.size();i++) {
         System.out.println("camera"+ i + " fps= " + fpslist.get(i)[Camera.Parameters.PREVIEW_FPS_MIN_INDEX]);
         System.out.println("camera"+ i + " fps= " + fpslist.get(i)[Camera.Parameters.PREVIEW_FPS_MAX_INDEX]);
        }

        List<Integer> format=parameters.getSupportedPreviewFormats();
        for (int i=0;i < format.size();i++) {
            System.out.println("format "+format.get(i));
        }
*/
        List<Camera.Size> sizes=parameters.getSupportedPreviewSizes();
        for (int i=0;i < sizes.size();i++) {
            System.out.println("preview size "+sizes.get(i).width+"x"+sizes.get(i).height);
        }

        parameters. setPreviewFpsRange(30000, 30000);

        android.hardware.Camera.Size preferredPreviewSize = parameters.getPreferredPreviewSizeForVideo ();
            
        int bitsPerPixel = android.graphics.ImageFormat.getBitsPerPixel(parameters.getPreviewFormat());

        parameters.setPreviewSize(width, height);
        camera.setParameters(parameters);
		
        camera.addCallbackBuffer(new byte[((width * height) * bitsPerPixel) / 8 + 1]);
        camera.addCallbackBuffer(new byte[((width * height) * bitsPerPixel) / 8 + 1]);
        camera.addCallbackBuffer(new byte[((width * height) * bitsPerPixel) / 8 + 1]);
        camera.addCallbackBuffer(new byte[((width * height) * bitsPerPixel) / 8 + 1]);
        camera.addCallbackBuffer(new byte[((width * height) * bitsPerPixel) / 8 + 1]);
        camera.addCallbackBuffer(new byte[((width * height) * bitsPerPixel) / 8 + 1]);
        camera.addCallbackBuffer(new byte[((width * height) * bitsPerPixel) / 8 + 1]);
        camera.addCallbackBuffer(new byte[((width * height) * bitsPerPixel) / 8 + 1]);
        camera.addCallbackBuffer(new byte[((width * height) * bitsPerPixel) / 8 + 1]);
        camera.addCallbackBuffer(new byte[((width * height) * bitsPerPixel) / 8 + 1]);

        camera.setPreviewCallbackWithBuffer(this);
        camera.startPreview();
    }	
	
    public void stopPreview () {
        camera.stopPreview();
        camera.setPreviewCallbackWithBuffer(null);
        camera.release();
        camera = null;
    }

    public void onPreviewFrame(byte[] data, Camera camera) {
       // long t=System.nanoTime();
        frameAvailable(data, width, height);
        camera.addCallbackBuffer(data);
       // t=System.nanoTime()-t;
       // System.out.println(t/1000000);
    }

    public static native void frameAvailable(byte[] NV21FrameData, int width, int height);
}
