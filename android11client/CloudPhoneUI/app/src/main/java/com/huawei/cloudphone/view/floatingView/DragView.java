package com.huawei.cloudphone.view.floatingView;

import android.animation.ValueAnimator;
import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.animation.DecelerateInterpolator;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.AppCompatImageView;

public class DragView extends AppCompatImageView implements View.OnTouchListener {
    // 最小滑动距离，区分点击事件
    private int touchSlop;

    // 触控处理,是否拦截触控事件
    private boolean intercepted;
    private float downX;
    private float downY;
    private float lastX;
    private float lastY;

    public void setIgnoreTouchEvent(boolean ignoreTouchEvent) {
        this.ignoreTouchEvent = ignoreTouchEvent;
    }

    // 是否忽略触控事件
    private boolean ignoreTouchEvent;

    // 释放自动回到边界
    private ValueAnimator releaseAnimator;

    public DragView(@NonNull Context context) {
        this(context, null);
    }

    public DragView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public DragView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        touchSlop = ViewConfiguration.get(context).getScaledTouchSlop();
        setOnTouchListener(this);
    }

    @Override
    public boolean onTouch(View view, MotionEvent event) {
        // 是否忽略触控事件
        if (ignoreTouchEvent || (releaseAnimator != null && releaseAnimator.isRunning())) {
            return false;
        }
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                intercepted = false;
                lastX = downX = event.getRawX();
                lastY = downY = event.getRawY();
                break;
            case MotionEvent.ACTION_MOVE:
                float dx = event.getRawX() - downX;
                float dy = event.getRawY() - downY;
                if (Math.abs(dx) >= touchSlop || Math.abs(dy) >= touchSlop) {
                    intercepted = true;
                }
                if (intercepted) {
                    moveMainButton(event.getRawX() - lastX, event.getRawY() - lastY);
                }
                lastX = event.getRawX();
                lastY = event.getRawY();
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                if (intercepted) {
                    releaseView();
                }
                break;

        }
        return intercepted;
    }

    private void releaseView() {
        final boolean isLeft = isLeft();
        releaseAnimator = ValueAnimator.ofFloat(1.0f, 0);
        releaseAnimator.setDuration(300);
        releaseAnimator.setInterpolator(new DecelerateInterpolator());
        releaseAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animator) {
                float animatedValue = (float) animator.getAnimatedValue();
                if (isLeft) {
                    moveMainButton((getLeft() + getTranslationX()) * (animatedValue - 1), 0);
                } else {
                    moveMainButton(getTranslationX() * (animatedValue - 1), 0);
                }
            }
        });
        releaseAnimator.start();
    }

    public boolean isLeft() {
        return -getTranslationX() > (getLeft() + getWidth()) / 2f;
    }

    /**
     * 移动转主按钮
     *
     * @param dx distanceX
     * @param dy distanceY
     */
    private void moveMainButton(float dx, float dy) {
        float translationX = getTranslationX() + dx;
        if (dx > 0) {
            //右边界
            if (translationX > 0) {
                translationX = 0;
            }
        } else {
            if (translationX < -getLeft()) {
                translationX = -getLeft();
            }
        }
        // 限制纵向移动不能超出边界
        float translationY = getTranslationY() + dy;
        if (dy < 0) {
            if (translationY < -(getTop() - getMeasuredHeight() / 2f)) {
                translationY = -(getTop() - getMeasuredHeight() / 2f);
            }
        } else {
            if (translationY > getTop() - getMeasuredHeight()) {
                translationY = getTop() - getMeasuredHeight();
            }
        }
        setTranslationX(translationX);
        setTranslationY(translationX);
    }
}
