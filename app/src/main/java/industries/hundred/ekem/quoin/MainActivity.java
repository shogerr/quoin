package industries.hundred.ekem.quoin;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.app.Activity;
import android.os.Bundle;
import android.view.MotionEvent;

public class MainActivity extends Activity {

    private GLSurfaceView mGLView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mGLView = new MainGLSurfaceView(this);
        setContentView(mGLView);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        mGLView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mGLView.onResume();
    }

    static {
        System.loadLibrary("demo-renderer");
    }
}

class MainGLSurfaceView extends GLSurfaceView {

    public MainGLSurfaceView(Context context) {
        super(context);
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);
        setRenderer(new Renderer());
    }

    @Override
    public boolean performClick() {
        handleTouch();
        return super.performClick();
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        switch (e.getAction()) {
            case MotionEvent.ACTION_UP:
                performClick();
        }
        return true;
    }


    private static class Renderer implements GLSurfaceView.Renderer {
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            init();
        }

        public void onSurfaceChanged(GL10 gl, int w, int h) {

        }

        public void onDrawFrame(GL10 gl10) {
           nativeRenderer();
        }

    }

    private static native void handleTouch();
    private static native void nativeRenderer();
    private static native void init();
}
