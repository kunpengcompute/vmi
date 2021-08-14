/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.widget;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import com.huawei.instructionstream.appui.R;

/**
 * @author cly
 * @since 2020-03-24
 */
public class SettingItemsView extends RelativeLayout {
    private ImageView itemLeftIcon;
    private ImageView itemRightIcon;
    private View itemDivider;
    private TextView itemLeftText;
    private TextView itemRightText;

    /**
     * settingItemsView
     *
     * @param context Context
     */
    public SettingItemsView(Context context) {
        super(context);
        init(context, null);
    }

    /**
     * settingItemsView
     *
     * @param context Context
     * @param attrs   AttributeSet
     */
    public SettingItemsView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context, attrs);
    }

    /**
     * settingItemsView
     *
     * @param context      Context
     * @param attrs        AttributeSet
     * @param defStyleAttr int
     */
    public SettingItemsView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context, attrs);
    }

    /**
     * init
     *
     * @param context Context
     * @param attrs   AttributeSet
     */
    private void init(Context context, AttributeSet attrs) {
        LayoutInflater.from(context).inflate(R.layout.setting_item_view, this);

        itemLeftIcon = findViewById(R.id.item_left_icon);
        itemRightIcon = findViewById(R.id.item_right_icon);
        itemDivider = findViewById(R.id.item_divider);
        itemLeftText = findViewById(R.id.icon_left_text);
        itemRightText = findViewById(R.id.item_right_text);

        TypedArray ta = context.obtainStyledAttributes(attrs, R.styleable.settingItemView);
        boolean isShowDivider = ta.getBoolean(R.styleable.settingItemView_showDivider, false);
        boolean isShowLeftIcon = ta.getBoolean(R.styleable.settingItemView_showLeftIcon, false);
        itemDivider.setVisibility(isShowDivider ? View.VISIBLE : View.GONE);
        if (isShowLeftIcon) {
            itemLeftIcon.setVisibility(View.VISIBLE);
        } else {
            itemLeftIcon.setVisibility(View.GONE);
        }
        int leftIcon = ta.getResourceId(R.styleable.settingItemView_leftIcon, -1);
        int rightIcon = ta.getResourceId(R.styleable.settingItemView_rightIcon, R.mipmap.ic_arrow_right);
        if (leftIcon != -1) {
            itemLeftIcon.setImageResource(leftIcon);
        }
        String leftText = ta.getString(R.styleable.settingItemView_leftText);
        String rightText = ta.getString(R.styleable.settingItemView_rightText);
        itemRightIcon.setImageResource(rightIcon);
        itemLeftText.setText(leftText);
        itemRightText.setText(rightText);
        ta.recycle();
    }

    /**
     * setRightValue
     *
     * @param value String
     */
    public void setRightValue(String value) {
        itemRightText.setText(value);
    }

    /**
     * getRightText
     *
     * @return String
     */
    public String getRightText() {
        return itemRightText.getText().toString();
    }
}
