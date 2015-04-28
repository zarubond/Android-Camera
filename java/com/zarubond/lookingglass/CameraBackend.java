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

public class CameraBackend implements SurfaceTexture.OnFrameAvailableListener {

    private Camera camera;
    private static SurfaceTexture surface;
	
    public CameraBackend(){
    }	

    public void startPreview (int width, int height, int textureId) {

        surface=new SurfaceTexture(textureId);

        camera = Camera.open();

        surface.setOnFrameAvailableListener(this);

        try
        {
            camera.setPreviewTexture(surface);
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

        List<Camera.Size> sizes=parameters.getSupportedPreviewSizes();
        for (int i=0;i < sizes.size();i++) {
            System.out.println("preview size "+sizes.get(i).width+"x"+sizes.get(i).height);
        }
*/

        //http://stackoverflow.com/questions/14131900/extreme-camera-lag-on-nexus-4
        parameters.setRecordingHint(true);
        parameters.setPreviewFpsRange(30000, 30000);


        android.hardware.Camera.Size preferredPreviewSize = parameters.getPreferredPreviewSizeForVideo ();

        parameters.setPreviewSize(width, height);
        camera.setParameters(parameters);
        camera.startPreview();
    }
	
    public void stopPreview () {
        camera.stopPreview();
        camera.setPreviewCallbackWithBuffer(null);
        camera.release();
        surface.release();
        camera = null;
    }

    public void onFrameAvailable (SurfaceTexture surfaceTexture)
    {
        frameAvailable();
    }

    public void updateFrame()
    {
        synchronized(this) {
            surface.updateTexImage();
        }
    }

    public static native void frameAvailable();
}
