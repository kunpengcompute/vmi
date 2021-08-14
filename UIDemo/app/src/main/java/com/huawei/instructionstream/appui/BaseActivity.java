/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui;

import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import com.huawei.instructionstream.appui.widget.CustomToolBar;


/**
 * BaseActivity.
 *
 * @since 2018-07-05
 */
public abstract class BaseActivity extends AppCompatActivity {
    private static final String TAG = "CGP-BaseActivity";

    /**
     * CGPApplication.
     */
    protected CGPApplication application;

    /**
     * Context.
     */
    protected Context context;

    /**
     * CustomToolBar.
     */
    protected CustomToolBar toolBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(getLayoutId());
        if (getApplicationContext() instanceof CGPApplication) {
            application = (CGPApplication) getApplicationContext();
        }
        context = this;
        if (findViewById(R.id.toolbar) instanceof CustomToolBar) {
            toolBar = (CustomToolBar) findViewById(R.id.toolbar);
            setSupportActionBar(toolBar);
        }
        initViews();
        loadData();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    /**
     * getLayoutId.
     *
     * @return layoutid
     */
    protected abstract int getLayoutId();

    /**
     * initViews
     */
    protected abstract void initViews();

    /**
     * loadData
     */
    protected abstract void loadData();

    @Override
    protected void onResume() {
        super.onResume();
    }
}
