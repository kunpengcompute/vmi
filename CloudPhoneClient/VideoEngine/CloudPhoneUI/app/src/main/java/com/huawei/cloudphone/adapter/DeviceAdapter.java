/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.adapter;

import android.content.Context;
import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.adapter.holder.DeviceViewHolder;
import com.huawei.cloudphone.model.DeviceListBean;
import com.huawei.cloudphone.util.ListUtil;

import java.util.List;

/**
 * DeviceAdapter
 *
 * @author zhh
 * @since 2019-09-24
 */
public class DeviceAdapter extends RecyclerView.Adapter<DeviceViewHolder> {
    private final LayoutInflater mInflater;
    private final List<DeviceListBean.DeviceBean> mDeviceList;

    private OnDeviceItemClickListener mOnDeviceItemClickListener;

    /**
     * DeviceAdapter
     *
     * @param context    Context
     * @param deviceList List<DeviceListBean.DeviceBean>
     */
    public DeviceAdapter(Context context, List<DeviceListBean.DeviceBean> deviceList) {
        mDeviceList = deviceList;
        mInflater = LayoutInflater.from(context);
    }

    @NonNull
    @Override
    public DeviceViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View itemView = mInflater.inflate(R.layout.item_device_list, parent, false);
        return new DeviceViewHolder(itemView, mOnDeviceItemClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull DeviceViewHolder holder, int position) {
        DeviceListBean.DeviceBean deviceBean = mDeviceList.get(position);
        holder.setValue(deviceBean);
    }

    @Override
    public int getItemCount() {
        return ListUtil.getSize(mDeviceList);
    }

    /**
     * bind click listener
     *
     * @param onDeviceItemClickListener listener
     */
    public void setOnDeviceItemClickListener(OnDeviceItemClickListener onDeviceItemClickListener) {
        mOnDeviceItemClickListener = onDeviceItemClickListener;
    }

    /**
     * Item点击回调
     */
    public interface OnDeviceItemClickListener {
        /**
         * 点击重启
         *
         * @param view     View
         * @param position int
         */
        void onRebootClicked(View view, int position);

        /**
         * 点击删除
         *
         * @param view     View
         * @param position int
         */
        void onDeleteClicked(View view, int position);

        /**
         * 接入设备
         *
         * @param view     View
         * @param position int
         */
        void onItemClicked(View view, int position);
    }
}
