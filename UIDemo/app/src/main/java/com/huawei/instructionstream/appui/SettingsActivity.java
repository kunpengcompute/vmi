/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui;

import android.os.Bundle;
import android.view.View;
import com.huawei.instructionstream.appui.common.RadioDialog;
import com.huawei.instructionstream.appui.model.SettingsBean;
import com.huawei.instructionstream.appui.utils.SPUtil;
import com.huawei.instructionstream.appui.widget.SettingItemsView;

/**
 * 设置页
 *
 * @since 2021-06-17
 */
public class SettingsActivity extends BaseActivity {
    private SettingItemsView netSettingIemtView;
    private String[] netTypeArr = {"TCP", "nStack"};
    private int mselectedNetTypeIndex = -1;
    private SettingsBean settingsBean = new SettingsBean();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected int getLayoutId() {
        return R.layout.activity_settings;
    }

    @Override
    protected void initViews() {
        netSettingIemtView = (SettingItemsView) findViewById(R.id.item_net_type);
    }

    @Override
    protected void loadData() {
        getDataFromSP();
        toolBar.setLeftButtonIcon(R.mipmap.ic_back);
        toolBar.setTitleText("设置");
        toolBar.setLeftButtonOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
        netSettingIemtView.setRightValue(settingsBean.getNetTypeName());
        setSelectednetTypeIndex(settingsBean.getNetType());
        netSettingIemtView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showDialog("网络类型", netTypeArr, mselectedNetTypeIndex,
                        new RadioDialog.OnRadioDialogItemSelectedListener() {
                            @Override
                            public void onRadioDialogItemSelected(int position) {
                                setSelectednetTypeIndex(position);
                                netSettingIemtView.setRightValue(netTypeArr[position]);
                                settingsBean.setNetType(position);
                                settingsBean.setNetTypeName(netTypeArr[position]);
                                SPUtil.putObject(SPUtil.INS_SETTING, settingsBean);
                            }
                        });
            }
        });
    }

    private void getDataFromSP() {
        settingsBean = SPUtil.getObject(SPUtil.INS_SETTING,SettingsBean.class,settingsBean);
    }

    private void showDialog(String title, String[] items, int choosedIndex,
                            RadioDialog.OnRadioDialogItemSelectedListener itemChoosedListener) {
        RadioDialog.Builder builder = new RadioDialog.Builder(SettingsActivity.this);
        builder.setTitle(title);
        builder.setItems(items);
        builder.setChoosedIndex(choosedIndex);
        builder.setOnRadioDialogItemChoosedListener(itemChoosedListener);
        builder.create().show();
    }

    /**
     * 选择网络类型
     *
     * @param selectedNetTypeIndex int
     */
    public void setSelectednetTypeIndex(int selectedNetTypeIndex) {
        this.mselectedNetTypeIndex = selectedNetTypeIndex;
    }
}