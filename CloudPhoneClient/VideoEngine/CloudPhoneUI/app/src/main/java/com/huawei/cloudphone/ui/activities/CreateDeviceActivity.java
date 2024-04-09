/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.ui.activities;

import android.content.DialogInterface;
import androidx.appcompat.app.AlertDialog;

import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.model.RegionListBean;
import com.huawei.cloudphone.model.TemplateBean;
import com.huawei.cloudphone.util.CastUtil;
import com.huawei.cloudphone.util.ClickUtil;
import com.huawei.cloudphone.util.ListUtil;
import com.huawei.cloudphone.util.ToastUtil;
import com.huawei.cloudphone.util.ViewUtil;
import com.huawei.cloudphone.widget.TopBar;

import java.util.ArrayList;
import java.util.List;

/**
 * 申请设备页
 *
 * @author gj
 * @since 2019-09-24
 */
public class CreateDeviceActivity extends BaseActivity implements View.OnClickListener {
    private static final String TAG = CreateDeviceActivity.class.getSimpleName();
    // 标识是选择区域的弹窗
    private static final int FLAG_REGION = 0;

    // 标识是选择模板的弹窗
    private static final int FLAG_TEMPLATE = 1;

    // 3特指视频流类型的手机
    private static final int TYPE_CLOUD_PHONE = 3;

    // 导航栏
    private TopBar mTopBar;

    // 设备名称
    private EditText mDeviceNameEditText;

    // 区域
    private LinearLayout mRegionLinearLayout;
    private TextView mRegionTextView;

    // 模板
    private TextView mTemplateTextView;
    private LinearLayout mTemplateLinearLayout;

    // 申请设备
    private Button mApplyDeviceButton;

    // 区域信息
    private String[] mRegionArray;

    // 当前选中的区域index
    private int mSelectedRegionIndex = -1;

    // 模板信息
    private String[] mTemplateArray;

    // 当前选中的模板index
    private int mSelectedTemplateIndex = -1;

    // 模板数据
    private List<TemplateBean> mTemplateBeans;

    @Override
    protected int getLayoutRes() {
        return R.layout.activity_create_device;
    }

    @Override
    protected void initView() {
        mTopBar = CastUtil.toTopBar(findViewById(R.id.tb_header));
        mDeviceNameEditText = CastUtil.toEditText(findViewById(R.id.et_device_name));
        mRegionLinearLayout = CastUtil.toLinearLayout(findViewById(R.id.ll_region));
        mRegionTextView = CastUtil.toTextView(findViewById(R.id.tv_region));
        mTemplateLinearLayout = CastUtil.toLinearLayout(findViewById(R.id.ll_template));
        mTemplateTextView = CastUtil.toTextView(findViewById(R.id.tv_template));
        mApplyDeviceButton = CastUtil.toButton(findViewById(R.id.btn_apply_device));
    }

    @Override
    protected void setListener() {
        // 导航栏点击
        mTopBar.setOnTopBarClickListener(new TopBar.OnTopBarClickListener() {
            @Override
            public void onBackClicked(View view) {
                finish();
            }

            @Override
            public void onMenuClicked(View view) {
            }
        });
        // 区域、模板、申请设备点击
        mRegionLinearLayout.setOnClickListener(this);
        mTemplateLinearLayout.setOnClickListener(this);
        mApplyDeviceButton.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        if (ClickUtil.isFastClick()) {
            return;
        }
        switch (view.getId()) {
            case R.id.ll_region: // 选择区域
                if (mRegionArray == null || mRegionArray.length == 0) {
                    ToastUtil.showToast("暂无可选区域");
                    return;
                }
                showChooseDialog(FLAG_REGION, mRegionArray, mSelectedRegionIndex, mRegionTextView);
                break;
            case R.id.ll_template: // 选择模板
                if (mSelectedRegionIndex < 0) {
                    ToastUtil.showToast("请先选择区域");
                    return;
                }
                handleTemplateData();
                if (mTemplateArray == null || mTemplateArray.length == 0) {
                    ToastUtil.showToast("暂无可选模板");
                    return;
                }
                showChooseDialog(FLAG_TEMPLATE, mTemplateArray, mSelectedTemplateIndex, mTemplateTextView);
                break;
            case R.id.btn_apply_device: { // 申请设备:
                break;
            }
            default: {
                break;
            }
        }
    }

    /**
     * 显示弹窗
     *
     * @param flag          标识是区域弹窗还是模板弹窗
     * @param data          弹窗列表数据
     * @param selectedIndex 选中的位置
     * @param textView      显示选中的文字
     */
    private void showChooseDialog(final int flag, final String[] data,
        final int selectedIndex, final TextView textView) {
        AlertDialog alertDialog = new AlertDialog.Builder(mActivity)
            .setSingleChoiceItems(data, selectedIndex, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    if (flag == FLAG_REGION) {
                        mSelectedRegionIndex = which;
                        mTemplateTextView.setText("");
                        mSelectedTemplateIndex = -1;
                    } else {
                        if (flag == FLAG_TEMPLATE) {
                            mSelectedTemplateIndex = which;
                        }
                    }
                    ViewUtil.setText(textView, data[which]);
                    dialog.dismiss();
                }
            }).create();
        alertDialog.show();
    }

    /**
     * 将区域信息转为数字供区域弹窗调用
     *
     * @param regionBeans 区域数据
     */
    private void handleRegionData(List<RegionListBean.RegionBean> regionBeans) {
        mRegionArray = new String[regionBeans.size()];
        for (int i = 0; i < regionBeans.size(); i++) {
            RegionListBean.RegionBean regionBean = regionBeans.get(i);
            String name = regionBean.getName();
            String alias = "";
            List<RegionListBean.RegionBean.ClustersBean> clustersBeans = regionBean.getClusters();
            if (clustersBeans != null && clustersBeans.size() > 0) {
                alias = clustersBeans.get(0).getAlias();
            }
            mRegionArray[i] = name + "-" + alias;
        }
    }

    /**
     * 根据选择的区域找模板
     */
    private void handleTemplateData() {
        if (ListUtil.isEmpty(mTemplateBeans)) {
            mTemplateArray = null;
            return;
        }
        List<String> templateStringList = new ArrayList<>();
        for (int i = 0; i < mTemplateBeans.size(); i++) {
            TemplateBean templateBean = mTemplateBeans.get(i);
            TemplateBean.Image image = templateBean.getImage();
            String regionName = image.getRegionName();
            String regionAlias = image.getRegionAlias();

            if (mRegionArray[mSelectedRegionIndex].equals(regionName + "-" + regionAlias)) {
                templateStringList.add(templateBean.getName());
            }
        }
        mTemplateArray = templateStringList.toArray(new String[0]);
    }
}
