/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.floating;

import android.animation.ValueAnimator;
import android.annotation.SuppressLint;
import android.content.Context;
import androidx.annotation.Nullable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.animation.DecelerateInterpolator;
import androidx.appcompat.widget.AppCompatImageView;

import com.huawei.cloudphonesdk.utils.LogUtil;

/**
 * Class instruction
 *
 * @author zWX809008
 * @since 2020/5/28
 */
public class DragView extends AppCompatImageView implements View.OnTouchListener {
    private static final String TAG = DragView.class.getSimpleName();

    // 最小滑动距离，区分点击事件
    private int mTouchSlop;

    // 触控处理，是否拦截触控事件
    private boolean mIntercepted;
    private float mDownX;
    private float mDownY;
    private float mLastX;
    private float mLastY;

    // 是否忽略触控事件
    private boolean mIgnoreTouchEvent;

    // 释放自动回到边界动画
    private ValueAnimator mReleaseAnimator;

    /**
     * Constructor
     *
     * @param context Context
     */
    public DragView(Context context) {
        this(context, null);
    }

    /**
     * Constructor
     *
     * @param context Context
     * @param attrs   AttributeSet
     */
    public DragView(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Constructor
     *
     * @param context      Context
     * @param attrs        AttributeSet
     * @param defStyleAttr defStyleAttr
     */
    public DragView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mTouchSlop = ViewConfiguration.get(context).getScaledTouchSlop();
        setOnTouchListener(this);
    }

    @SuppressLint("ClickableViewAccessibility")
    @Override
    public boolean onTouch(View view, MotionEvent event) {
        // 是否忽略触控事件
        if (mIgnoreTouchEvent || (mReleaseAnimator != null && mReleaseAnimator.isRunning())) {
            return false;
        }
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                mIntercepted = false;
                mLastX = mDownX = event.getRawX();
                mLastY = mDownY = event.getRawY();
                break;
            case MotionEvent.ACTION_MOVE:
                float dx = event.getRawX() - mDownX;
                float dy = event.getRawY() - mDownY;
                if (Math.abs(dx) >= mTouchSlop || Math.abs(dy) >= mTouchSlop) {
                    // 滑动距离超过最小滑动单位，认为是滑动事件，不响应点击事件
                    mIntercepted = true;
                }
                // 如果响应滑动事件，更新控件位置
                if (mIntercepted) {
                    moveMainButton(event.getRawX() - mLastX, event.getRawY() - mLastY);
                }
                mLastX = event.getRawX();
                mLastY = event.getRawY();
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                if (mIntercepted) {
                    releaseView();
                }
                break;
        }
        return mIntercepted;
    }

    /**
     * 移动主按钮
     *
     * @param dx distanceX
     * @param dy distanceY
     */
    private void moveMainButton(float dx, float dy) {
        // 限制横向移动不能超出边界
        float translationX = getTranslationX() + dx;
        if (dx > 0) { // 右边界
            if (translationX > 0) {
                translationX = 0;
            }
        } else { // 左边界
            if (translationX < -getLeft()) {
                translationX = -getLeft();
            }
        }
        // 限制纵向移动不能超出边界
        float translationY = getTranslationY() + dy;
        if (dy < 0) { // 上边界
            if (translationY < -(getTop() - getMeasuredHeight() / 2f)) {
                translationY = -(getTop() - getMeasuredHeight() / 2f);
            }
        } else { // 下边界
            if (translationY > getTop() - getMeasuredHeight()) {
                translationY = getTop() - getMeasuredHeight();
            }
        }
        setTranslationX(translationX);
        setTranslationY(translationY);
    }

    /**
     * 释放自动回到边界
     */
    private void releaseView() {
        final boolean isLeft = isLeft();
        // 释放的时候自动回到边界动画
        mReleaseAnimator = ValueAnimator.ofFloat(1.0f, 0);
        mReleaseAnimator.setDuration(300);
        mReleaseAnimator.setInterpolator(new DecelerateInterpolator());
        mReleaseAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                float animatedValue = (float) animation.getAnimatedValue();
                if (isLeft) {
                    // 回到左边
                    LogUtil.info(TAG, "回到左边");
                    moveMainButton((getLeft() + getTranslationX()) * (animatedValue - 1), 0);
                } else {
                    // 回到右边
                    LogUtil.info(TAG, "回到右边");
                    moveMainButton(getTranslationX() * (animatedValue - 1), 0);
                }
            }
        });
        mReleaseAnimator.start();
    }

    /**
     * 判断在左边还是有右边
     *
     * @return true:isLeft
     */
    public boolean isLeft() {
        return -getTranslationX() > (getLeft() + getWidth()) / 2f;
    }

    /**
     * 是否忽略触控事件,展开的时候或者动画未执行完不处理触控事件
     *
     * @param ignoreTouchEvent boolean
     */
    public void setIgnoreTouchEvent(boolean ignoreTouchEvent) {
        mIgnoreTouchEvent = ignoreTouchEvent;
    }
}
