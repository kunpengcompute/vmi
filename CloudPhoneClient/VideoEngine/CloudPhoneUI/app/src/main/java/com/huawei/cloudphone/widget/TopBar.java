/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.widget;

import android.content.Context;
import android.content.res.TypedArray;
import androidx.annotation.DrawableRes;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.CastUtil;
import com.huawei.cloudphone.util.ClickUtil;
import com.huawei.cloudphone.util.ViewUtil;

/**
 * TopBar
 *
 * @author zhh
 * @since 2019-09-24
 */
public class TopBar extends FrameLayout {
    // 返回返回键
    private final ImageView mBackImageView;

    // 标题
    private final TextView mTitleTextView;

    // 菜单键
    private final ImageView mMenuImageView;

    // 点击事件
    private OnTopBarClickListener mOnTopBarClickListener;

    /**
     * TopBar构造函数
     *
     * @param context Context
     */
    public TopBar(@NonNull Context context) {
        this(context, null);
    }

    /**
     * TopBar构造函数
     *
     * @param context Context
     * @param attrs   AttributeSet
     */
    public TopBar(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * TopBar构造函数
     *
     * @param context      Context
     * @param attrs        AttributeSet
     * @param defStyleAttr int
     */
    public TopBar(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        LayoutInflater.from(context).inflate(R.layout.layout_top_bar, this);
        mBackImageView = CastUtil.toImageView(findViewById(R.id.iv_back));
        mTitleTextView = CastUtil.toTextView(findViewById(R.id.tv_title));
        mMenuImageView = CastUtil.toImageView(findViewById(R.id.iv_menu));

        // 自定义属性
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.TopBar);
        int backImageRes = typedArray.getResourceId(R.styleable.TopBar_tb_back, 0);
        String title = typedArray.getString(R.styleable.TopBar_tb_title);
        int menuImageRes = typedArray.getResourceId(R.styleable.TopBar_tb_menu, 0);
        typedArray.recycle();

        setBackImage(backImageRes);
        setTitle(title);
        setMenuImage(menuImageRes);

        setListener();
    }

    private void setListener() {
        // 返回键点击
        mBackImageView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mOnTopBarClickListener != null && !ClickUtil.isFastClick()) {
                    mOnTopBarClickListener.onBackClicked(view);
                }
            }
        });
        // 菜单键点击
        mMenuImageView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mOnTopBarClickListener != null && !ClickUtil.isFastClick()) {
                    mOnTopBarClickListener.onMenuClicked(view);
                }
            }
        });
    }

    /**
     * 设置返回键图标
     *
     * @param imageRes 图标Resource类型
     */
    private void setBackImage(@DrawableRes int imageRes) {
        if (imageRes == 0) {
            ViewUtil.setVisibility(mBackImageView, View.GONE);
        } else {
            ViewUtil.setVisibility(mBackImageView, View.VISIBLE);
            mBackImageView.setImageResource(imageRes);
        }
    }

    /**
     * 设置标题
     *
     * @param title 标题String类型
     */
    private void setTitle(String title) {
        if (TextUtils.isEmpty(title)) {
            ViewUtil.setVisibility(mTitleTextView, View.GONE);
        } else {
            ViewUtil.setVisibility(mTitleTextView, View.VISIBLE);
            ViewUtil.setText(mTitleTextView, title);
        }
    }

    /**
     * 设置菜单键图标
     *
     * @param imageRes 图标Resource类型
     */
    private void setMenuImage(@DrawableRes int imageRes) {
        if (imageRes == 0) {
            ViewUtil.setVisibility(mMenuImageView, View.GONE);
        } else {
            ViewUtil.setVisibility(mMenuImageView, View.VISIBLE);
            mMenuImageView.setImageResource(imageRes);
        }
    }

    /**
     * 添加点击事件
     *
     * @param onTopBarClickListener 点击事件
     */
    public void setOnTopBarClickListener(OnTopBarClickListener onTopBarClickListener) {
        mOnTopBarClickListener = onTopBarClickListener;
    }

    /**
     * 顶部导航栏点击事件
     */
    public interface OnTopBarClickListener {
        /**
         * 点击返回键
         *
         * @param view 返回键View
         */
        void onBackClicked(View view);

        /**
         * 点击菜单键
         *
         * @param view 菜单键View
         */
        void onMenuClicked(View view);
    }
}
