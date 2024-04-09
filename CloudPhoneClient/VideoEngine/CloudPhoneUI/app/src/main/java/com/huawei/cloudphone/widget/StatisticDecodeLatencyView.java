/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.widget;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.huawei.cloudphonesdk.utils.ThreadPool;

import java.util.concurrent.CopyOnWriteArrayList;

/**
 * 解码延时统计表格
 *
 * @author zWX809008
 * @since 2020/5/30
 */
public class StatisticDecodeLatencyView extends SurfaceView implements Runnable, SurfaceHolder.Callback {
    /**
     * I Frame
     */
    public static final int FRAME_TYPE_I = 103;

    /**
     * P Frame
     */
    public static final int FRAME_TYPE_P = 97;

    /**
     * 绘制内容左右边距
     */
    private static final int PADDING_TOP = 20;

    /**
     * 绘制内容上下边距
     */
    private static final int PADDING_LEFT = 60;

    /**
     * Y轴分为5部分
     */
    private static final int Y_NUM = 5;

    // 设置数据间隔
    private int mItemGap;

    // 设置数据量
    private int mItemCount;

    // 最大值
    private int mMaxValue;

    // 标题
    private String mTitle;

    // 是否绘制数据值
    private boolean mDrawData;

    // 帧数据
    private CopyOnWriteArrayList<FrameData> mDataList = new CopyOnWriteArrayList<>();

    // 绘制线程
    private ThreadPool mDrawThreadPool;
    private boolean mThreadStop;
    private Paint mPaint;

    /**
     * 构造方法
     *
     * @param context Context
     */
    public StatisticDecodeLatencyView(Context context) {
        this(context, null);
    }

    /**
     * 构造方法
     *
     * @param context Context
     */
    public StatisticDecodeLatencyView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * 构造方法
     *
     * @param context      Context
     * @param attrs        属性
     * @param defStyleAttr 默认样式
     */
    public StatisticDecodeLatencyView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mPaint.setStyle(Paint.Style.FILL);
        mPaint.setTextSize(14);
        getHolder().addCallback(this);
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        mDrawThreadPool = new ThreadPool(4, 20);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mThreadStop = false;
        mDrawThreadPool.submit(this);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        mThreadStop = true;
    }

    @Override
    protected void onDetachedFromWindow() {
        getHolder().removeCallback(this);
        super.onDetachedFromWindow();
    }

    /**
     * 设置帧数据
     *
     * @param frameType 帧类型
     * @param frameData 帧数据
     */
    public void setData(int frameType, int frameData) {
        if (mDataList.size() >= mItemCount) {
            mDataList.remove(0);
        }
        mDataList.add(new FrameData(frameType, frameData));
    }

    @Override
    public void run() {
        while (!mThreadStop) {
            Canvas canvas = getHolder().lockCanvas();
            if (canvas != null) {
                drawFrameData(canvas);
                getHolder().unlockCanvasAndPost(canvas);
            }
            sleep(14);
        }
    }

    /**
     * 画帧数据
     *
     * @param canvas Canvas
     */
    private synchronized void drawFrameData(Canvas canvas) {
        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
        // 画边线
        mPaint.setColor(Color.BLACK);
        canvas.drawLine(PADDING_LEFT, PADDING_TOP, PADDING_LEFT,
            getHeight() - PADDING_TOP, mPaint);
        canvas.drawLine(PADDING_LEFT, getHeight() - PADDING_TOP,
            getWidth() - PADDING_LEFT, getHeight() - PADDING_TOP, mPaint);
        // 画Y轴
        mPaint.setColor(Color.BLACK);
        mPaint.setTextAlign(Paint.Align.RIGHT);
        float itemHeight = (getHeight() - PADDING_TOP * 2) * 1.0f / Y_NUM;
        float itemValue = mMaxValue * 1.0f / Y_NUM;
        for (int i = 1; i <= Y_NUM; i++) {
            float startX = PADDING_LEFT;
            float startY = getHeight() - PADDING_TOP - i * itemHeight;
            canvas.drawLine(startX, startY, startX + 2, startY, mPaint);
            canvas.drawText(String.valueOf(i * (int) itemValue), startX - 4, startY + 14 / 2, mPaint);
        }
        // 画数据
        mPaint.setTextAlign(Paint.Align.CENTER);
        float itemWidth = (getWidth() - mItemGap * mItemCount - PADDING_LEFT * 2) * 1.0f / mItemCount;
        float startX = (PADDING_LEFT + itemWidth * (mItemCount - mDataList.size()));
        for (int i = 0; i < mDataList.size(); i++) {
            FrameData frameData = mDataList.get(i);
            float startY = getHeight() - PADDING_TOP - itemHeight * (frameData.mData * 1.0f / itemValue);
            mPaint.setColor(getFrameColor(frameData.mType));
            canvas.drawRect(startX, startY, startX + itemWidth, getHeight() - PADDING_TOP, mPaint);
            if (mDrawData) {
                canvas.drawText(String.valueOf(frameData.mData), startX + itemWidth / 2, startY, mPaint);
            }
            mPaint.setColor(Color.BLACK);
            startX += (itemWidth + mItemGap);
        }
        // 画标题
        mPaint.setColor(Color.BLACK);
        if (!TextUtils.isEmpty(mTitle)) {
            canvas.drawText(mTitle, getWidth() / 2, getHeight() - 4, mPaint);
        }
    }

    /**
     * 不同帧使用不同的颜色
     *
     * @param type 帧类型
     * @return Color
     */
    private int getFrameColor(int type) {
        switch (type) {
            case FRAME_TYPE_I:
                return 0xFF17CAFF;
            case FRAME_TYPE_P:
                return 0xFFF6D681;
            default:
                return Color.GRAY;
        }
    }

    /**
     * Thread.sleep
     *
     * @param millis millis
     */
    private void sleep(long millis) {
        try {
            Thread.sleep(millis);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * 设置标题
     *
     * @param title 标题
     */
    public void setTitle(String title) {
        mTitle = title;
    }

    /**
     * 设置最大值
     *
     * @param maxValue 最大值
     */
    public void setMaxValue(int maxValue) {
        mMaxValue = maxValue;
    }

    /**
     * getMaxValue
     *
     * @return mMaxValue
     */
    public int getMaxValue() {
        return mMaxValue;
    }

    /**
     * 设置统计多少帧数据
     *
     * @param itemCount itemCount
     */
    public void setItemCount(int itemCount) {
        mItemCount = itemCount;
    }

    /**
     * 设置矩形间隔
     *
     * @param itemGap itemGap
     */
    public void setItemGap(int itemGap) {
        mItemGap = itemGap;
    }

    /**
     * 设置是否绘制数值
     *
     * @param drawData drawData
     */
    public void setDrawData(boolean drawData) {
        mDrawData = drawData;
    }

    /**
     * 帧数据包装类
     */
    private static class FrameData {
        int mType;
        int mData;

        FrameData(int type, int data) {
            mType = type;
            mData = data;
        }
    }
}