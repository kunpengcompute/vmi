/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.widget;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.support.annotation.Nullable;
import android.support.v7.widget.TintTypedArray;
import android.support.v7.widget.Toolbar;
import android.util.AttributeSet;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.huawei.instructionstream.appui.R;

/**
 * CustomToolBar.
 *
 * @since 2018-07-05
 */
public class CustomToolBar extends Toolbar {
    private String TAG = "CustomToolBar";
    private LayoutInflater mInflater;
    private View mView;
    private TextView mTextTitle;
    private EditText mSearchView;
    private Button mLeftButton;
    private Button mRightButton;

    /**
     * CustomToolBar.
     *
     * @param context context
     */
    public CustomToolBar(Context context) {
        this(context, null);
    }

    /**
     * CustomToolBar.
     *
     * @param context context
     * @param attrs attrs
     */
    public CustomToolBar(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    @SuppressLint("RestrictedApi")
    /**
     * CustomToolBar.
     *
     * @param context context
     * @param attrs attrs
     * @param defStyleAttr defStyleAttr
     */
    public CustomToolBar(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initView();
        // 设置toolbar的边距
        setContentInsetsRelative(10, 10);
        if (attrs != null) {
            final TintTypedArray tintTypes = TintTypedArray.obtainStyledAttributes(getContext(), attrs,
                    R.styleable.CustomToolBar, defStyleAttr, 0);
            // 自定义控件的模式：1.通过TintTypedArray类从attrs.xml中取得自定义属性值
            // 如果属性值非空，将其赋值
            final Drawable leftIcon = tintTypes.getDrawable(R.styleable.CustomToolBar_leftButtonIcon);
            // 一定要在这里进行条件判断
            if (leftIcon != null) {
                setLeftButtonIcon(leftIcon);
            }
            final Drawable rightIcon = tintTypes.getDrawable(R.styleable.CustomToolBar_rightButtonIcon);
            // 一定要在这里进行条件判断
            if (rightIcon != null) {
                setRightButtonIcon(rightIcon);
            }
            // 默认false
            boolean isShowSearchView = tintTypes.getBoolean(R.styleable.CustomToolBar_isShowSearchView, false);
            // 如果isShowSearchView为true，把Title隐藏
            if (isShowSearchView) {
                showSearchView();
                hideTitleView();
            }
            CharSequence rightButtonText = tintTypes.getText(R.styleable.CustomToolBar_rightButtonText);
            if (rightButtonText != null) {
                setRightButtonText(rightButtonText);
            }
            tintTypes.recycle();
        }
    }

    private void initView() {
        if (mView == null) {
            mInflater = LayoutInflater.from(getContext());
            mView = mInflater.inflate(R.layout.custom_toolbar, null);
            if (mView.findViewById(R.id.toolbar_title) instanceof TextView) {
                mTextTitle = (TextView) mView.findViewById(R.id.toolbar_title);
            }
            if (mView.findViewById(R.id.toolbar_searchview) instanceof EditText) {
                mSearchView = (EditText) mView.findViewById(R.id.toolbar_searchview);
                mSearchView.setFocusable(false);
                mSearchView.setOnClickListener(new OnClickListener() {
                    @Override
                    public void onClick(View v) {
                    }
                });
            }
            if (mView.findViewById(R.id.toolbar_leftButton) instanceof Button) {
                mLeftButton = (Button) mView.findViewById(R.id.toolbar_leftButton);
            }
            if (mView.findViewById(R.id.toolbar_rightButton) instanceof Button) {
                mRightButton = (Button) mView.findViewById(R.id.toolbar_rightButton);
            }
            // 把Toolbar里面的控件组合起来
            LayoutParams lp = new LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT,
                    Gravity.CENTER_HORIZONTAL);
            addView(mView, lp);
        }
    }

    /**
     * setLeftButtonIcon.
     *
     * @param icon icon
     */
    public void setLeftButtonIcon(Drawable icon) {
        if (mLeftButton != null) {
            mLeftButton.setBackground(icon);
            mLeftButton.setVisibility(VISIBLE);
        }
    }

    /**
     * setRightButtonIcon.
     *
     * @param icon icon
     */
    public void setRightButtonIcon(Drawable icon) {
        if (mRightButton != null) {
            mRightButton.setBackground(icon);
            mRightButton.setVisibility(VISIBLE);
        }
    }

    /**
     * 提供外接方法，实现EditView和TextView的转换
     */
    public void showSearchView() {
        if (mSearchView != null) {
            mSearchView.setVisibility(VISIBLE);
        }
    }

    /**
     * hideTitleView.
     */
    public void hideTitleView() {
        if (mTextTitle != null) {
            mTextTitle.setVisibility(GONE);
        }
    }

    /**
     * setRightButtonText.
     *
     * @param text text
     */
    public void setRightButtonText(CharSequence text) {
        mRightButton.setText(text);
        mRightButton.setVisibility(VISIBLE);
    }

    /**
     * setLeftButtonIcon
     *
     * @param icon icon
     */
    public void setLeftButtonIcon(int icon) {
        setLeftButtonIcon(getResources().getDrawable(icon));
    }

    /**
     * setRightButtonIcon
     *
     * @param icon icon
     */
    public void setRightButtonIcon(int icon) {
        setRightButtonIcon(getResources().getDrawable(icon));
    }

    /**
     * setLeftButtonOnClickListener
     *
     * @param listener listener
     */
    public void setLeftButtonOnClickListener(OnClickListener listener) {
        mLeftButton.setOnClickListener(listener);
    }

    /**
     * setRightButtonOnClickListener.
     *
     * @param listener listener
     */
    public void setRightButtonOnClickListener(OnClickListener listener) {
        mRightButton.setOnClickListener(listener);
    }

    /**
     * setRightButtonText.
     *
     * @param id resources id
     */
    public void setRightButtonText(int id) {
        setRightButtonText(getResources().getString(id));
    }

    /**
     * 设置标题.
     *
     * @param resId resId
     */
    public void setTitleText(int resId) {
        setTitleText(getContext().getText(resId));
    }

    /**
     * setTitleText.
     *
     * @param title title
     */
    public void setTitleText(CharSequence title) {
        if (mTextTitle != null) {
            mTextTitle.setVisibility(VISIBLE);
            mTextTitle.setText(title);
        }
    }

    /**
     * setTitleTextSize.
     *
     * @param size size
     */
    public void setTitleTextSize(int size) {
        if (mTextTitle != null) {
            mTextTitle.setTextSize(size);
            mTextTitle.setGravity(Gravity.CENTER);
        }
    }

    /**
     * hideSearchView
     */
    public void hideSearchView() {
        if (mSearchView != null) {
            mSearchView.setVisibility(GONE);
        }
    }

    /**
     * showTitleView
     */
    public void showTitleView() {
        if (mTextTitle != null) {
            mTextTitle.setVisibility(VISIBLE);
        }
    }
}
