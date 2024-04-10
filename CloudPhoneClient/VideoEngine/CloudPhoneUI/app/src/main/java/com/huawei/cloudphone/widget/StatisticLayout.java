/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.widget;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.FrameLayout;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.CastUtil;

/**
 * 包裹统计控件的布局
 *
 * @author zWX809008
 * @since 2020/1/10
 */
public class StatisticLayout extends FrameLayout {
    /**
     * 动画时长
     */
    private static final long ANIMATION_DURATION = 300;

    // 帧率、带宽、I帧统计
    private StatisticDecodeFpsView mDecodeFrameRateView;
    private StatisticDecodeFpsView mBandWidthView;
    private StatisticDecodeLatencyView mFrameSizeView;
    private StatisticDecodeLatencyView mFrameTimeView;
    private StatisticTouchLatencyView mTouchTimeView;

    // 是否开启了菜单
    private boolean mIsOpen = true;

    // 动画是否结束，避免动画执行过程中再次点击重复执行
    private volatile boolean mIsAnimationEnd = true;

    // 打开关闭动画
    private Animation mOpenAnimation;
    private Animation mCloseAnimation;

    /**
     * 构造方法
     *
     * @param context Context
     */
    public StatisticLayout(Context context) {
        this(context, null);
    }

    /**
     * 构造方法
     *
     * @param context Context
     * @param attrs   属性
     */
    public StatisticLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * 构造方法
     *
     * @param context      Context
     * @param attrs        属性
     * @param defStyleAttr 默认样式
     */
    public StatisticLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        LayoutInflater.from(context).inflate(R.layout.layout_statistic, this);
        initView();
    }

    private void initView() {
        // 解码帧率统计
        mDecodeFrameRateView = CastUtil.toStatisticDecodeFpsView(findViewById(R.id.decodeFrameRate));
        mDecodeFrameRateView.setTitle("解码帧率统计(fps)");
        mDecodeFrameRateView.setMaxValue(60);
        mDecodeFrameRateView.setYCount(6);
        // 带宽统计
        mBandWidthView = CastUtil.toStatisticDecodeFpsView(findViewById(R.id.bandWidth));
        mBandWidthView.setTitle("实时带宽统计(kb/s)");
        mBandWidthView.setMaxValue(1000);
        mBandWidthView.setYCount(5);
        // I帧统计
        mFrameSizeView = CastUtil.toStatisticDecodeLatencyView(findViewById(R.id.iFrame));
        mFrameSizeView.setTitle("I帧统计(bit)");
        mFrameSizeView.setItemCount(30); // 保存最近30帧数据
        mFrameSizeView.setMaxValue(10000);
        mFrameSizeView.setItemGap(2);
        mFrameSizeView.setZOrderOnTop(true);
        // 解码延迟
        mFrameTimeView = CastUtil.toStatisticDecodeLatencyView(findViewById(R.id.frameTime));
        mFrameTimeView.setTitle("解码延迟(ms)");
        mFrameTimeView.setItemCount(30); // 保存最近30帧数据
        mFrameTimeView.setMaxValue(200);
        mFrameTimeView.setItemGap(2);
        mFrameTimeView.setDrawData(true);
        mFrameTimeView.setZOrderOnTop(true);
        // 触控一秒钟延迟
        mTouchTimeView = CastUtil.toStatisticTouchLatencyView(findViewById(R.id.frameTouchTime));
        mTouchTimeView.setTitle("触控后一秒钟延迟(ms)");
        mTouchTimeView.setItemGap(2);
    }

    /**
     * 刷新统计数据
     *
     * @param decodeFrameRate 解码帧率
     * @param bandWidth       带宽
     */
    public void refreshStatisticData(int decodeFrameRate, int bandWidth) {
        mDecodeFrameRateView.refreshData(decodeFrameRate);
        mBandWidthView.refreshData(bandWidth);
        // 动态设置带宽的最大值,每次增加的跨度为500
        int maxBrandValue = (bandWidth + 500) / 500 * 500;
        if (maxBrandValue > mBandWidthView.getMaxValue()) {
            mBandWidthView.setMaxValue(maxBrandValue);
        }
    }

    /**
     * 刷新帧数据
     *
     * @param frameType 帧类型
     * @param frameSize 帧大小
     * @param frameTime 帧时间戳
     */
    public void refreshFrameData(int frameType, int frameSize, int frameTime) {
        final int frameSizeUnit = 1000;
        // 动态设置I帧的最大值
        int maxFrameSizeValue = (frameSize / frameSizeUnit + 1) * frameSizeUnit;
        if (maxFrameSizeValue > mFrameSizeView.getMaxValue()) {
            mFrameSizeView.setMaxValue(maxFrameSizeValue);
        }
        mFrameSizeView.setData(frameType, frameSize);

        final int frameTimeUnit = 50;
        // 动态设置maxFrameTimeValue
        int maxFrameTimeValue = (frameTime / frameTimeUnit + 1) * frameTimeUnit;
        if (maxFrameTimeValue > mFrameTimeView.getMaxValue()) {
            mFrameTimeView.setMaxValue(maxFrameTimeValue);
        }
        mFrameTimeView.setData(0, frameTime);
    }

    /**
     * 设置触控时间
     *
     * @param touchTime touchTime
     */
    public void setTouchTime(long touchTime) {
        mTouchTimeView.setTouchTime(touchTime);
    }

    /**
     * 刷新触控之后的解码延时数据
     *
     * @param frameType 帧类型
     * @param frameSize 帧大小
     * @param frameTime 解码延时
     */
    public void refreshFrameTime(int frameType, int frameSize, long frameTime) {
        mTouchTimeView.addData(frameType, frameSize, frameTime);
    }

    /**
     * 开始绘制触控延时数据
     */
    public void startDraw() {
        mTouchTimeView.startDraw();
    }

    /**
     * 判断面板是否打开
     *
     * @return true:打开 false:关闭
     */
    public boolean isOpen() {
        return mIsOpen;
    }

    /**
     * 打开面板
     */
    public void openBoard() {
        if (mIsOpen || !mIsAnimationEnd) {
            return;
        }
        mIsAnimationEnd = false;
        if (mOpenAnimation == null) {
            mOpenAnimation = new TranslateAnimation(
                Animation.RELATIVE_TO_SELF, -1.0f,
                Animation.RELATIVE_TO_SELF, 0,
                Animation.RELATIVE_TO_SELF, 0,
                Animation.RELATIVE_TO_SELF, 0);
            mOpenAnimation.setDuration(ANIMATION_DURATION);
            mOpenAnimation.setFillAfter(true);
            mOpenAnimation.setInterpolator(new AccelerateInterpolator());
            mOpenAnimation.setAnimationListener(new Animation.AnimationListener() {
                @Override
                public void onAnimationStart(Animation animation) {
                }

                @Override
                public void onAnimationEnd(Animation animation) {
                    mIsAnimationEnd = true;
                    mIsOpen = true;
                }

                @Override
                public void onAnimationRepeat(Animation animation) {
                }
            });
        }
        startAnimation(mOpenAnimation);
    }

    /**
     * 关闭面板
     */
    public void closeBoard() {
        if (!mIsOpen || !mIsAnimationEnd) {
            return;
        }
        mIsAnimationEnd = false;
        if (mCloseAnimation == null) {
            mCloseAnimation = new TranslateAnimation(
                Animation.RELATIVE_TO_SELF, 0,
                Animation.RELATIVE_TO_SELF, -1.0f,
                Animation.RELATIVE_TO_SELF, 0,
                Animation.RELATIVE_TO_SELF, 0);
            mCloseAnimation.setDuration(ANIMATION_DURATION);
            mCloseAnimation.setFillAfter(true);
            mCloseAnimation.setInterpolator(new AccelerateInterpolator());
            mCloseAnimation.setAnimationListener(new Animation.AnimationListener() {
                @Override
                public void onAnimationStart(Animation animation) {
                }

                @Override
                public void onAnimationEnd(Animation animation) {
                    mIsAnimationEnd = true;
                    mIsOpen = false;
                }

                @Override
                public void onAnimationRepeat(Animation animation) {
                }
            });
        }
        startAnimation(mCloseAnimation);
    }

    @Override
    public void setVisibility(int visibility) {
        super.setVisibility(visibility);
        mFrameSizeView.setVisibility(visibility);
        mFrameTimeView.setVisibility(visibility);
    }
}