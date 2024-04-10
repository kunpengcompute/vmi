/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.floating;

import android.animation.Animator;
import android.animation.ValueAnimator;
import android.annotation.SuppressLint;
import androidx.annotation.DrawableRes;
import androidx.annotation.IdRes;
import android.view.Gravity;
import android.view.View;
import android.view.animation.DecelerateInterpolator;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.ClickUtil;
import com.huawei.cloudphonesdk.utils.LogUtil;

import java.util.ArrayList;
import java.util.List;

/**
 * Class instruction
 *
 * @author zWX809008
 * @since 2020/4/7
 */
public class FloatingManager {
    private static final String TAG = FloatingManager.class.getSimpleName();

    @SuppressLint("StaticFieldLeak")
    private FrameLayout mContentView;

    // 主按钮
    private DragView mDragView;
    private int mMenuSize;

    // 菜单按钮
    private List<ImageView> mSubImageViews = new ArrayList<>();
    private float mMenuRadius;

    // 标识是否open
    private boolean mIsOpen;

    // 展开，关闭动画
    private ValueAnimator mValueAnimator;

    /**
     * 设置按钮图片
     *
     * @param imgRes 图片
     * @return FloatingManager
     */
    @SuppressLint("ClickableViewAccessibility")
    public FloatingManager setMainImage(@DrawableRes int imgRes) {
        mDragView = new DragView(MyApplication.instance);
        mDragView.setImageResource(imgRes);
        mDragView.setScaleType(ImageView.ScaleType.FIT_XY);
        mDragView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (ClickUtil.isFastClick()) {
                    return;
                }
                setOpen(!mIsOpen);
            }
        });
        return this;
    }

    /**
     * 添加Item
     *
     * @param imgRes   图片
     * @param id       View.id
     * @param listener 点击事件
     * @return FloatingManager
     */
    public FloatingManager addMenuItem(@DrawableRes int imgRes, @IdRes int id, View.OnClickListener listener) {
        ImageView imageView = new ImageView(MyApplication.instance);
        imageView.setImageResource(imgRes);
        imageView.setId(id);
        imageView.setOnClickListener(listener);
        mSubImageViews.add(0, imageView);
        return this;
    }

    /**
     * 依附一个ViewGroup
     *
     * @param contentView FrameLayout
     * @return FloatingManager
     */
    public FloatingManager attach(FrameLayout contentView) {
        mContentView = contentView;
        return this;
    }

    /**
     * 创建菜单
     */
    public void create() {
        if (mContentView == null) {
            throw new IllegalStateException("need attach to a content view");
        }
        mMenuSize = MyApplication.instance.getResources().getDimensionPixelOffset(R.dimen.floating_width);
        mMenuRadius = MyApplication.instance.getResources().getDimensionPixelOffset(R.dimen.floating_radius);
        addMainButton();
        if (mIsOpen) {
            addSubButton();
        }
    }

    /**
     * 添加主按钮
     */
    private void addMainButton() {
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(mMenuSize, mMenuSize);
        params.gravity = Gravity.END | Gravity.CENTER_VERTICAL;
        mContentView.addView(mDragView, params);
    }

    /**
     * 添加子菜单
     */
    private void addSubButton() {
        for (int i = 0; i < mSubImageViews.size(); i++) {
            ImageView imageView = mSubImageViews.get(i);
            FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(mMenuSize, mMenuSize);
            params.leftMargin = (int) (mContentView.getWidth() - mDragView.getWidth()
                + mDragView.getTranslationX());
            params.topMargin = (int) ((mContentView.getHeight() - mDragView.getHeight()) / 2f
                + mDragView.getTranslationY());
            mContentView.addView(imageView, params);
        }
    }

    /**
     * 移除子菜单
     */
    private void removeSubButton() {
        for (ImageView imageView : mSubImageViews) {
            mContentView.removeView(imageView);
        }
    }

    /**
     * isOpen
     *
     * @return isOpen
     */
    public boolean isOpen() {
        return mIsOpen;
    }

    /**
     * Set FloatingView Open
     *
     * @param open open
     */
    public void setOpen(boolean open) {
        if (mIsOpen == open || (mValueAnimator != null && mValueAnimator.isRunning())) {
            return;
        }
        mIsOpen = open;
        mDragView.setIgnoreTouchEvent(mIsOpen);
        if (mIsOpen) {
            LogUtil.info(TAG, "open menu");
            addSubButton();
            startSelfAnimation(null);
        } else {
            LogUtil.info(TAG, "close menu");
            startSelfAnimation(new Animator.AnimatorListener() {
                @Override
                public void onAnimationStart(Animator animation) {
                }

                @Override
                public void onAnimationEnd(Animator animation) {
                    removeSubButton();
                }

                @Override
                public void onAnimationCancel(Animator animation) {
                }

                @Override
                public void onAnimationRepeat(Animator animation) {
                }
            });
        }
    }

    /**
     * 展开、关闭动画
     *
     * @param animatorListener 动画回调
     */
    private void startSelfAnimation(Animator.AnimatorListener animatorListener) {
        mValueAnimator = ValueAnimator.ofFloat(0, 1.0f);
        mValueAnimator.setDuration(300);
        mValueAnimator.setInterpolator(new DecelerateInterpolator());
        if (animatorListener != null) {
            mValueAnimator.addListener(animatorListener);
        }
        mValueAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                // 动画进度
                float animatedValue = (float) animation.getAnimatedValue();
                boolean isLeft = mDragView.isLeft();
                for (int i = 0; i < mSubImageViews.size(); i++) {
                    ImageView imageView = mSubImageViews.get(i);
                    // 角度转弧度
                    float angle = (float) (Math.PI / (mSubImageViews.size() + 1)) * (i + 1);
                    // 根据半径计算横纵坐标
                    float xDistance = (float) (mMenuRadius * Math.sin(angle));
                    float yDistance = (float) (mMenuRadius * Math.cos(angle));
                    if (isLeft) {
                        imageView.setTranslationX(xDistance * (mIsOpen ? animatedValue : (1 - animatedValue)));
                    } else {
                        imageView.setTranslationX(-xDistance * (mIsOpen ? animatedValue : (1 - animatedValue)));
                    }
                    imageView.setTranslationY(yDistance * (mIsOpen ? animatedValue : (1 - animatedValue)));
                }
            }
        });
        mValueAnimator.start();
    }

    public void setColor(int res) {
        mDragView.setImageResource(res);
    }
}
