package com.huawei.cloudphone.view.floatingView;

import android.animation.Animator;
import android.animation.ValueAnimator;
import android.view.Gravity;
import android.view.View;
import android.view.animation.DecelerateInterpolator;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.example.cloudphoneui.R;
import com.huawei.cloudphone.MyApplication;
import com.huawei.utils.Utils;

import java.util.ArrayList;
import java.util.List;

public class FloatingManger {
    private FrameLayout mContentView;

    private DragView mDragView;
    private int mMenuSize;

    private List<ImageView> mSubImageViews = new ArrayList<>();
    private float mMenuRadius;
    private boolean mIsOpen;
    private ValueAnimator mValueAnimator;

    public FloatingManger setMainImage(int imaRes) {
        mDragView = new DragView(MyApplication.instance);
        mDragView.setImageResource(imaRes);
        mDragView.setScaleType(ImageView.ScaleType.FIT_XY);
        mDragView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (Utils.isFastClick()) {
                    return;
                }
                setOpen(!mIsOpen);
            }
        });
        return this;
    }

    public void setOpen(boolean b) {
        if (mIsOpen == b || (mValueAnimator != null && mValueAnimator.isRunning())) {
            return;
        }
        mIsOpen = b;
        mDragView.setIgnoreTouchEvent(mIsOpen);
        if (mIsOpen) {
            addSubButton();
            startSelfAnimation(null);
        } else {
            startSelfAnimation(new Animator.AnimatorListener() {
                @Override
                public void onAnimationStart(Animator animator) {

                }

                @Override
                public void onAnimationEnd(Animator animator) {
                    removeSubButton();
                }

                @Override
                public void onAnimationCancel(Animator animator) {

                }

                @Override
                public void onAnimationRepeat(Animator animator) {

                }
            });
        }
    }

    private void removeSubButton() {
        for (ImageView imageView : mSubImageViews) {
            mContentView.removeView(imageView);
        }
    }


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
                float animatedValue = (float) animation.getAnimatedValue();
                boolean isLeft = mDragView.isLeft();
                for (int i = 0; i < mSubImageViews.size(); i++) {
                    ImageView imageView = mSubImageViews.get(i);
                    float angle = (float) (Math.PI / (mSubImageViews.size() + 1)) * (i + 1);
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

    public void create() {
        if (mContentView == null) {
            throw new IllegalStateException("need attach to a content view");
        }
        mMenuSize = MyApplication.instance.getResources().getDimensionPixelOffset(R.dimen.floating_with);
        mMenuRadius = MyApplication.instance.getResources().getDimensionPixelOffset(R.dimen.floating_radius);
        addMainButton();
        if (mIsOpen) {
            addSubButton();
        }
    }

    private void addMainButton() {
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(mMenuSize, mMenuSize);
        params.gravity = Gravity.END | Gravity.CENTER_VERTICAL;
        mContentView.addView(mDragView, params);
    }

    public FloatingManger addMenuItem(int imgRes, int id, View.OnClickListener listener) {
        ImageView imageView = new ImageView(MyApplication.instance);
        imageView.setImageResource(imgRes);
        imageView.setId(id);
        imageView.setOnClickListener(listener);
        mSubImageViews.add(0, imageView);
        return this;
    }

    private void addSubButton() {
        for (int i = 0; i < mSubImageViews.size(); i++) {
            ImageView imageView = mSubImageViews.get(i);
            FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(mMenuSize, mMenuSize);
            params.leftMargin = (int) (mContentView.getWidth() - mDragView.getWidth()
                    + mDragView.getTranslationX());
            params.topMargin = (int) ((mContentView.getHeight() - mDragView.getHeight()) / 2f + mDragView.getTranslationY());
            mContentView.addView(imageView, params);
        }
    }

    public FloatingManger attach(FrameLayout container) {
        mContentView = container;
        return this;
    }

    public void setColor(int res) {
        mDragView.setImageResource(res);
    }

    public boolean isOpen() {
        return mIsOpen;
    }
}
