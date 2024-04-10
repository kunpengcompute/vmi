/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.widget;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.View;

import java.util.concurrent.CopyOnWriteArrayList;

/**
 * 触控后一秒延迟统计
 *
 * @author zWX809008
 * @since 2020/5/30
 */
public class StatisticTouchLatencyView extends View {
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

    // 最大值
    private int mMaxSizeValue = 10000;
    private int mMaxTimeValue = 1000;

    // 标题
    private String mTitle;

    private long mTouchTime;

    // 帧数据
    private CopyOnWriteArrayList<FrameData> mDataList = new CopyOnWriteArrayList<>();
    private Paint mPaint;

    /**
     * 构造方法
     *
     * @param context Context
     */
    public StatisticTouchLatencyView(Context context) {
        this(context, null);
    }

    /**
     * 构造方法
     *
     * @param context Context
     */
    public StatisticTouchLatencyView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * 构造方法
     *
     * @param context      Context
     * @param attrs        属性
     * @param defStyleAttr 默认样式
     */
    public StatisticTouchLatencyView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mPaint.setStyle(Paint.Style.FILL);
        mPaint.setTextSize(14);
    }

    /**
     * 设置帧数据
     *
     * @param frameType 帧类型
     * @param frameSize 帧大小
     * @param frameTime 帧时间戳
     */
    public void addData(int frameType, int frameSize, long frameTime) {
        int time = (int) (frameTime - mTouchTime);
        if (time < 0) {
            return;
        }
        mDataList.add(new FrameData(frameType, frameSize, time));
        if (frameSize > mMaxSizeValue) {
            mMaxSizeValue = (frameSize / 1000 + 1) * 1000;
        }
    }

    /**
     * 设置触控时间
     *
     * @param touchTime touchTime
     */
    public void setTouchTime(long touchTime) {
        mDataList.clear();
        mMaxSizeValue = 0;
        mTouchTime = touchTime;
    }

    /**
     * 开始绘制
     */
    public void startDraw() {
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        drawFrameData(canvas);
        super.onDraw(canvas);
    }

    /**
     * 画帧数据
     *
     * @param canvas Canvas
     */
    private synchronized void drawFrameData(Canvas canvas) {
        // 画边线
        mPaint.setColor(Color.BLACK);
        canvas.drawLine(PADDING_LEFT, PADDING_TOP, PADDING_LEFT, getHeight() - PADDING_TOP, mPaint);
        canvas.drawLine(PADDING_LEFT, getHeight() - PADDING_TOP,
            getWidth() - PADDING_LEFT, getHeight() - PADDING_TOP, mPaint);
        canvas.drawLine(getWidth() - PADDING_LEFT, PADDING_TOP,
            getWidth() - PADDING_LEFT, getHeight() - PADDING_TOP, mPaint);
        // 画Y轴
        mPaint.setColor(Color.BLACK);
        float itemHeight = (getHeight() - PADDING_TOP * 2) * 1.0f / Y_NUM;
        float itemSizeValue = mMaxSizeValue * 1.0f / Y_NUM;
        float itemTimeValue = mMaxTimeValue * 1.0f / Y_NUM;
        for (int i = 1; i <= Y_NUM; i++) {
            mPaint.setTextAlign(Paint.Align.RIGHT);
            float startY = getHeight() - PADDING_TOP - i * itemHeight;
            canvas.drawLine(PADDING_LEFT, startY, PADDING_LEFT + 2, startY, mPaint);
            canvas.drawText(String.valueOf(i * (int) itemSizeValue),
                PADDING_LEFT - 4, startY + 14 / 2, mPaint);
            mPaint.setTextAlign(Paint.Align.LEFT);
            canvas.drawLine(getWidth() - PADDING_LEFT, startY,
                getWidth() - PADDING_LEFT - 2, startY, mPaint);
            canvas.drawText(String.valueOf(i * (int) itemTimeValue),
                getWidth() - PADDING_LEFT + 4, startY + 14 / 2, mPaint);
        }
        // 画数据
        mPaint.setTextAlign(Paint.Align.CENTER);
        int size = mDataList.size();
        float itemWidth = (getWidth() - mItemGap * size - PADDING_LEFT * 2) * 1.0f / size;
        float startX = PADDING_LEFT;
        for (int i = 0; i < mDataList.size(); i++) {
            FrameData frameData = mDataList.get(i);
            float startSizeY = getHeight() - PADDING_TOP
                - itemHeight * (frameData.mSize * 1.0f / itemSizeValue);
            float startTimeY = getHeight() - PADDING_TOP -
                itemHeight * (frameData.mTime * 1.0f / itemTimeValue);
            mPaint.setColor(getFrameColor(frameData.mType));
            canvas.drawRect(startX, startSizeY, startX + itemWidth,
                getHeight() - PADDING_TOP, mPaint);
            mPaint.setColor(Color.RED);
            canvas.drawCircle(startX + itemWidth / 2, startTimeY, 2, mPaint);
            mPaint.setTextSize(12);
            canvas.drawText(String.valueOf(frameData.mTime),
                startX + itemWidth / 2, startTimeY - 4, mPaint);
            startX += (itemWidth + mItemGap);
        }
        // 画标题
        mPaint.setColor(Color.BLACK);
        mPaint.setTextSize(14);
        if (!TextUtils.isEmpty(mTitle)) {
            canvas.drawText(mTitle, getWidth() / 2, getHeight() - 4, mPaint);
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
     * 设置标题
     *
     * @param title 标题
     */
    public void setTitle(String title) {
        mTitle = title;
    }

    /**
     * 设置item间隔
     *
     * @param itemGap itemGap
     */
    public void setItemGap(int itemGap) {
        mItemGap = itemGap;
    }

    /**
     * 帧数据包装类
     */
    private static class FrameData {
        int mType;
        int mSize;
        long mTime;

        FrameData(int type, int size, long time) {
            mType = type;
            mSize = size;
            mTime = time;
        }
    }
}
