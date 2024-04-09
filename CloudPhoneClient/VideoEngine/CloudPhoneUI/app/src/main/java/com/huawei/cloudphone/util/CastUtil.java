/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.util;

import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;
import androidx.recyclerview.widget.RecyclerView;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.ProgressBar;

import com.huawei.cloudphone.widget.StatisticDecodeFpsView;
import com.huawei.cloudphone.widget.StatisticDecodeLatencyView;
import com.huawei.cloudphone.widget.StatisticLayout;
import com.huawei.cloudphone.widget.StatisticTouchLatencyView;
import com.huawei.cloudphone.widget.VmiSurfaceView;
import com.huawei.cloudphonesdk.utils.InstanceofUtils;
import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.widget.TopBar;

/**
 * 工具类
 *
 * @author zhh
 * @since 2019-11-07
 */
public class CastUtil extends InstanceofUtils {
    /**
     * change to RecyclerView.
     *
     * @param input input param
     * @return T.
     */
    public static <T> RecyclerView toRecyclerView(T input) {
        if (input instanceof RecyclerView) {
            return (RecyclerView) input;
        } else {
            return new RecyclerView(MyApplication.instance);
        }
    }

    /**
     * change to TopBar.
     *
     * @param input input param
     * @return T.
     */
    public static <T> TopBar toTopBar(T input) {
        if (input instanceof TopBar) {
            return (TopBar) input;
        } else {
            return new TopBar(MyApplication.instance);
        }
    }

    /**
     * change to SwipeRefreshLayout.
     *
     * @param input input param
     * @return T.
     */
    public static <T> SwipeRefreshLayout toSwipeRefreshLayout(T input) {
        if (input instanceof SwipeRefreshLayout) {
            return (SwipeRefreshLayout) input;
        } else {
            return new SwipeRefreshLayout(MyApplication.instance);
        }
    }

    /**
     * change to CheckBox.
     *
     * @param input input param
     * @return T.
     */
    public static <T> CheckBox toCheckBox(T input) {
        if (input instanceof CheckBox) {
            return (CheckBox) input;
        } else {
            return new CheckBox(MyApplication.instance);
        }
    }

    /**
     * change to ViewGroup.
     *
     * @param input input param
     * @return T.
     */
    public static <T> ViewGroup toViewGroup(T input) {
        if (input instanceof ViewGroup) {
            return (ViewGroup) input;
        } else {
            throw new IllegalArgumentException("ViewGroup cast failed");
        }
    }

    /**
     * change to ProgressBar.
     *
     * @param input input param
     * @return T.
     */
    public static <T> ProgressBar toProgressBar(T input) {
        if (input instanceof ProgressBar) {
            return (ProgressBar) input;
        } else {
            throw new IllegalArgumentException("ProgressBar cast failed");
        }
    }

    /**
     * change to VmiSurfaceView.
     *
     * @param input input param
     * @return T.
     */
    public static <T> VmiSurfaceView toVmiSurfaceView(T input) {
        if (input instanceof VmiSurfaceView) {
            return (VmiSurfaceView) input;
        } else {
            throw new IllegalArgumentException("VmiSurfaceView cast failed");
        }
    }

    /**
     * change to SurfaceView.
     *
     * @param input input param
     * @return T.
     */
    public static <T> SurfaceView toSurfaceView(T input) {
        if (input instanceof SurfaceView) {
            return (SurfaceView) input;
        } else {
            throw new IllegalArgumentException("SurfaceView cast failed");
        }
    }

    /**
     * change to StatisticDecodeFpsView.
     *
     * @param input input param
     * @return T.
     */
    public static <T> StatisticDecodeFpsView toStatisticDecodeFpsView(T input) {
        if (input instanceof StatisticDecodeFpsView) {
            return (StatisticDecodeFpsView) input;
        } else {
            throw new IllegalArgumentException("StatisticDecodeFpsView cast failed");
        }
    }

    /**
     * change to StatisticDecodeLatencyView.
     *
     * @param input input param
     * @return T.
     */
    public static <T> StatisticDecodeLatencyView toStatisticDecodeLatencyView(T input) {
        if (input instanceof StatisticDecodeLatencyView) {
            return (StatisticDecodeLatencyView) input;
        } else {
            throw new IllegalArgumentException("StatisticDecodeLatencyView cast failed");
        }
    }

    /**
     * change to StatisticTouchLatencyView.
     *
     * @param input input param
     * @return T.
     */
    public static <T> StatisticTouchLatencyView toStatisticTouchLatencyView(T input) {
        if (input instanceof StatisticTouchLatencyView) {
            return (StatisticTouchLatencyView) input;
        } else {
            throw new IllegalArgumentException("StatisticTouchLatencyView cast failed");
        }
    }

    /**
     * change to StatisticLayout.
     *
     * @param input input param
     * @return T.
     */
    public static <T> StatisticLayout toStatisticLayout(T input) {
        if (input instanceof StatisticLayout) {
            return (StatisticLayout) input;
        } else {
            throw new IllegalArgumentException("StatisticLayout cast failed");
        }
    }
}
