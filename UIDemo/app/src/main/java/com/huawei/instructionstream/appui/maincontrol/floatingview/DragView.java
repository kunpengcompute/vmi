/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol.floatingview;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.huawei.instructionstream.appui.R;

import java.io.InputStream;

/**
 * DragView.
 *
 * @since 2018-07-05
 */
public class DragView extends View {
    private static Paint sPaint = new Paint();
    private static int resourceWidth = 120;
    private int viewHeight;
    private int viewWidth;
    private Rect rect;
    private Bitmap bitmap = null;
    private int deltaX = 0;
    private int deltaY = 0;
    private int startX = 0;
    private int startY = 0;
    private MagnetViewListener magnetViewListener;

    /**
     * constructor of context and attrs.
     *
     * @param context activity context
     * @param attrs attributes
     */
    public DragView(Context context, AttributeSet attrs) {
        super(context, attrs);
        sPaint = new Paint();
        resourceWidth = getResources().getDimensionPixelSize(R.dimen.ic_radius);
        rect = new Rect(0, 0, resourceWidth, resourceWidth);
    }

    /**
     * contructor.
     *
     * @param context the application context or activity context
     */
    public DragView(Context context) {
        super(context);
        sPaint = new Paint();
        resourceWidth = getResources().getDimensionPixelSize(R.dimen.ic_radius);
        rect = new Rect(0, 0, resourceWidth, resourceWidth);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int widthSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightSize = MeasureSpec.getSize(heightMeasureSpec);
        setMeasuredDimension(widthSize, heightSize);
        viewWidth = widthSize;
        viewHeight = heightSize;
        rect = new Rect(viewWidth - resourceWidth,
                        viewHeight / 2 - resourceWidth / 2,
                        viewWidth, viewHeight / 2 + resourceWidth / 2);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        if (bitmap != null) {
            Rect rectF = new Rect();
            rectF.set(0, 0, bitmap.getWidth(), bitmap.getHeight());
            canvas.drawBitmap(bitmap, rectF, rect, sPaint);
        } else {
            sPaint.setColor(Color.RED);
            canvas.drawRect(rect, sPaint);
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int axesX = (int)event.getX();
        int axesY = (int)event.getY();
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                if (!rect.contains(axesX, axesY)) {
                    return false;
                }
                startX = axesX;
                startY = axesY;
                deltaX = axesX - rect.left;
                deltaY = axesY - rect.top;
                break;
            case MotionEvent.ACTION_MOVE:
                rect.left = axesX - deltaX;
                if (rect.left < 0) {
                    rect.left = 0;
                }
                rect.top = axesY - deltaY;
                if (rect.top < 0) {
                    rect.top = 0;
                }
                rect.right = rect.left + resourceWidth;
                if (rect.right > viewWidth) {
                    rect.right = viewWidth;
                    rect.left = viewWidth - resourceWidth;
                }
                rect.bottom = rect.top + resourceWidth;
                if (rect.bottom > viewHeight) {
                    rect.bottom = viewHeight;
                    rect.top = viewHeight - resourceWidth;
                }
                Rect old = new Rect(rect);
                old.union(rect);
                invalidate(old);
                break;
            case MotionEvent.ACTION_UP:
                if (Math.abs(startX - axesX) < 10
                        && Math.abs(axesY - startY) < 10) {
                    if (magnetViewListener != null) {
                        magnetViewListener.onClick(this);
                    }
                }
                break;
            default:
                break;
        }
        return true;
    }

    private void moveToEdge(int yAxes) {
        if (rect.left + resourceWidth / 2 < viewWidth / 2) {
            rect.left = 0;
        } else {
            rect.left = viewWidth - resourceWidth;
        }
        rect.top = yAxes - deltaY;
        if (rect.top < 0) {
            rect.top = 0;
        }
        rect.right = rect.left + resourceWidth;
        rect.bottom = rect.top + resourceWidth;
        if (rect.bottom > viewHeight) {
            rect.bottom = viewHeight;
            rect.top = viewHeight - resourceWidth;
        }
        Rect old = new Rect(rect);
        old.union(rect);
        invalidate(old);
    }

    /**
     * set bg.
     *
     * @param resId the resource id
     */
    public void setbg(int resId) {
        if (bitmap != null) {
            bitmap.recycle();
            bitmap = null;
        }
        InputStream is = getContext().getResources().openRawResource(resId);
        Bitmap bmp = BitmapFactory.decodeStream(is);
        int bmpWidth = bmp.getWidth();
        int bmpHeight = bmp.getHeight();
        int width = resourceWidth;
        int height = resourceWidth;
        if (bmpWidth / width >= bmpHeight / height) {
            bitmap = Bitmap.createBitmap(bmp, (bmpWidth - bmpHeight) / 2, 0,
                width * bmpHeight / height, bmpHeight);
        } else {
            bitmap = Bitmap.createBitmap(bmp, 0, (bmpHeight - bmpWidth) / 2,
                bmpWidth, height * bmpWidth / width);
        }
        invalidate();
        bmp.recycle();
    }

    /**
     * setMagnetViewListener.
     *
     * @param magnetViewListener MagnetViewListener.
     */
    public void setMagnetViewListener(MagnetViewListener magnetViewListener) {
        this.magnetViewListener = magnetViewListener;
    }
}
