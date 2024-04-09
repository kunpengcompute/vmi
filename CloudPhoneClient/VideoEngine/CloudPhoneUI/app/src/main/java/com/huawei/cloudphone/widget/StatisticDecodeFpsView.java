/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.widget;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import androidx.annotation.Nullable;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.View;

import java.util.ArrayList;
import java.util.List;

/**
 * 解码帧率、带宽统计表格
 *
 * @author zWX809008
 * @since 2020/1/9
 */
public class StatisticDecodeFpsView extends View {
    // 最大值
    private int mMaxValue;

    // Y轴分几部分
    private int mYCount = 5;

    // X轴分几部分
    private int mXCount = 60;

    // 文字大小
    private int mTextSize = 12;

    // 上下边距
    private int mTopPadding = 20;

    // 左右边距
    private int mLeftPadding = 40;

    // 标题
    private String mTitle;

    private Paint mPaint;

    // 自身宽高
    private int mWidth;
    private int mHeight;
    private List<Integer> mFrameRateList = new ArrayList<>(mXCount);
    private Path mPath;

    /**
     * 构造方法
     *
     * @param context Context
     */
    public StatisticDecodeFpsView(Context context) {
        this(context, null);
    }

    /**
     * 构造方法
     *
     * @param context Context
     * @param attrs   属性
     */
    public StatisticDecodeFpsView(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * 构造方法
     *
     * @param context      Context
     * @param attrs        属性
     * @param defStyleAttr 默认样式
     */
    public StatisticDecodeFpsView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mPath = new Path();
        mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mPaint.setTextSize(mTextSize);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        if (mMaxValue > 0 && mYCount > 0 && mXCount > 0) {
            mWidth = getWidth();
            mHeight = getHeight();
            mPaint.setStyle(Paint.Style.FILL);
            mPaint.setColor(Color.BLACK);
            mPaint.setStrokeWidth(1);
            mPaint.setTextAlign(Paint.Align.RIGHT);
            mPath.reset();
            drawBorder(canvas);
            drawData(canvas);
        }
        super.onDraw(canvas);
    }

    private synchronized void drawBorder(Canvas canvas) {
        canvas.drawLine(mLeftPadding, mTopPadding, mLeftPadding,
            mHeight - mTopPadding, mPaint);
        canvas.drawLine(mLeftPadding, mHeight - mTopPadding,
            mWidth - mLeftPadding, mHeight - mTopPadding, mPaint);
    }

    private synchronized void drawData(Canvas canvas) {
        // 画Y轴
        float itemHeight = (mHeight - mTopPadding * 2) * 1.0f / mYCount;
        float itemValue = mMaxValue * 1.0f / mYCount;
        for (int i = 1; i <= mYCount; i++) {
            float startX = mLeftPadding;
            float startY = mHeight - mTopPadding - i * itemHeight;
            canvas.drawLine(startX, startY, startX + 2, startY, mPaint);
            canvas.drawText(String.valueOf(i * (int) itemValue), startX - 4, startY + mTextSize / 2, mPaint);
        }
        // 画数据
        mPaint.setTextAlign(Paint.Align.CENTER);
        mPaint.setColor(Color.RED);
        float itemWidth = (mWidth - mLeftPadding * 2) * 1.0f / mXCount;
        float startX = (mLeftPadding + itemWidth * (mXCount - mFrameRateList.size()));
        for (int i = 0; i < mFrameRateList.size(); i++) {
            int value = mFrameRateList.get(i);
            float startY = mHeight - mTopPadding - itemHeight * (value * 1.0f / itemValue);
            if (i == 0) {
                mPath.moveTo(startX, startY);
            } else {
                mPath.lineTo(startX, startY);
            }
            canvas.drawCircle(startX, startY, 1, mPaint);
            startX += itemWidth;
        }
        mPaint.setStrokeWidth(2);
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setColor(0xFF00FF49);
        canvas.drawPath(mPath, mPaint);
        // 画标题
        mPaint.setStrokeWidth(1);
        mPaint.setColor(Color.BLACK);
        mPaint.setStyle(Paint.Style.FILL);
        if (!TextUtils.isEmpty(mTitle)) {
            canvas.drawText(mTitle, mWidth / 2, mHeight - 4, mPaint);
        }
    }

    /**
     * 设置Y轴分成几份
     *
     * @param YCount 份数
     */
    public void setYCount(int YCount) {
        mYCount = YCount;
    }

    /**
     * 设置Y轴最大值
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
     * 设置表格标题
     *
     * @param title 表格标题
     */
    public void setTitle(String title) {
        mTitle = title;
    }

    /**
     * 添加数据
     *
     * @param data 数据
     */
    public synchronized void refreshData(int data) {
        if (mFrameRateList.size() >= mXCount) {
            mFrameRateList.remove(0);
        }
        mFrameRateList.add(data);
        invalidate();
    }
}