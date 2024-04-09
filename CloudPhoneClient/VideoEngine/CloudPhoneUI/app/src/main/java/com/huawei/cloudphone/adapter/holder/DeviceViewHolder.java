/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.adapter.holder;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.adapter.DeviceAdapter;
import com.huawei.cloudphone.model.DeviceListBean;
import com.huawei.cloudphone.util.CastUtil;
import com.huawei.cloudphone.util.ClickUtil;
import com.huawei.cloudphone.util.CommonUtil;
import com.huawei.cloudphone.util.ViewUtil;

import java.io.File;

/**
 * DeviceViewHolder
 *
 * @author zhh
 * @since 2019-09-24
 */
public class DeviceViewHolder extends RecyclerView.ViewHolder {
    // 设备名称
    private final TextView mNameTextView;

    // 设备状态
    private final TextView mStatusTextView;

    // 设备规格
    private final TextView mSpecTextView;

    // 缩略图
    private final ImageView mSnapImageView;

    // 重启
    private final ImageButton mRebootImageButton;

    // 删除
    private final ImageButton mDeleteImageButton;

    // 点击回调
    private final DeviceAdapter.OnDeviceItemClickListener mListener;

    /**
     * Holder
     *
     * @param itemView View
     * @param listener itemClickListener
     */
    public DeviceViewHolder(@NonNull View itemView, DeviceAdapter.OnDeviceItemClickListener listener) {
        super(itemView);
        mListener = listener;
        mNameTextView = CastUtil.toTextView(itemView.findViewById(R.id.tv_name));
        mStatusTextView = CastUtil.toTextView(itemView.findViewById(R.id.tv_status));
        mSpecTextView = CastUtil.toTextView(itemView.findViewById(R.id.tv_spec));
        mSnapImageView = CastUtil.toImageView(itemView.findViewById(R.id.iv_snap));
        mRebootImageButton = CastUtil.toImageButton(itemView.findViewById(R.id.ib_reboot));
        mDeleteImageButton = CastUtil.toImageButton(itemView.findViewById(R.id.ib_delete));
    }

    /**
     * set device info
     *
     * @param deviceBean device info
     */
    public void setValue(DeviceListBean.DeviceBean deviceBean) {
        String status = (1 == deviceBean.getDeviceVmStatus() ? "在线" : "离线");
        String onlineTime = CommonUtil.timeMillsToString(deviceBean.getDeviceOnlineTime());
        String ram = deviceBean.getDeviceRam();
        if (ram != null && ram.endsWith("M")) {
            String ramStr = ram.substring(0, ram.length() - 1);
            int ramInt = CommonUtil.stringToInt(ramStr, 0) / 1024;
            ram = ramInt + "G";
        }

        ViewUtil.setText(mNameTextView, deviceBean.getDeviceAlias());
        ViewUtil.setText(mStatusTextView, status + "-" + onlineTime);
        ViewUtil.setText(mSpecTextView, ram + File.separator + deviceBean.getDeviceStorage()
            + File.separator + deviceBean.getDeviceResolution());
        // 加载缩略图（静态图）
        mSnapImageView.setImageResource(R.drawable.img_screen_shot);
        // 重启点击事件
        mRebootImageButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mListener != null && !ClickUtil.isFastClick()) {
                    mListener.onRebootClicked(view, getAdapterPosition());
                }
            }
        });
        // 删除点击事件
        mDeleteImageButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mListener != null && !ClickUtil.isFastClick()) {
                    mListener.onDeleteClicked(view, getAdapterPosition());
                }
            }
        });

        // 接入游戏
        itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mListener != null && !ClickUtil.isFastClick()) {
                    mListener.onItemClicked(view, getAdapterPosition());
                }
            }
        });
    }
}
