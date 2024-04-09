/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.utils;

import android.os.Parcelable;
import androidx.annotation.Keep;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.ImageView;
import android.widget.ImageButton;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.LinearLayout;

import java.util.ArrayList;

/**
 * InstanceofUtils.
 *
 * @since 2019-08-09
 */
@Keep
public class InstanceofUtils {
    private static final String TAG = "InstanceofUtils";

    /**
     * change to String.
     *
     * @param input input param
     * @return T.
     */
    public static <T> String toInstanceString(T input) {
        if (input instanceof String) {
            return (String) input;
        } else {
            LogUtil.error(TAG, "T instanceof String failed");
            String sequence = null;
            return sequence;
        }
    }

    /**
     * change to TextView.
     *
     * @param input input param
     * @return T.
     */
    public static <T> TextView toTextView(T input) {
        if (input instanceof TextView) {
            return (TextView) input;
        } else {
            LogUtil.error(TAG, "T instanceof TextView failed");
            TextView textView = null;
            return textView;
        }
    }

    /**
     * change to EditText.
     *
     * @param input input param
     * @return T.
     */
    public static <T> EditText toEditText(T input) {
        if (input instanceof EditText) {
            return (EditText) input;
        } else {
            LogUtil.info(TAG, "T instanceof EditText error");
            EditText editText = null;
            return editText;
        }
    }

    /**
     * change to Button.
     *
     * @param input input param
     * @return T.
     */
    public static <T> Button toButton(T input) {
        if (input instanceof Button) {
            return (Button) input;
        } else {
            LogUtil.info(TAG, "T instanceof Button error");
            Button button = null;
            return button;
        }
    }

    /**
     * change to ImageButton.
     *
     * @param input input param
     * @return T.
     */
    public static <T> ImageButton toImageButton(T input) {
        if (input instanceof ImageButton) {
            return (ImageButton) input;
        } else {
            LogUtil.info(TAG, "T instanceof ImageButton error");
            ImageButton imageButton = null;
            return imageButton;
        }
    }

    /**
     * change to ImageView.
     *
     * @param input input param
     * @return T.
     */
    public static <T> ImageView toImageView(T input) {
        if (input instanceof ImageView) {
            return (ImageView) input;
        } else {
            LogUtil.info(TAG, "T instanceof ImageView error");
            ImageView imageView = null;
            return imageView;
        }
    }

    /**
     * change to LinearLayout.
     *
     * @param input input param
     * @return T.
     */
    public static <T> LinearLayout toLinearLayout(T input) {
        if (input instanceof LinearLayout) {
            return (LinearLayout) input;
        } else {
            LogUtil.info(TAG, "T instanceof LinearLayout error");
            LinearLayout layout = null;
            return layout;
        }
    }

    /**
     * change to FrameLayout.
     *
     * @param input input param
     * @return T.
     */
    public static <T> FrameLayout toFrameLayout(T input) {
        if (input instanceof FrameLayout) {
            return (FrameLayout) input;
        } else {
            LogUtil.info(TAG, "T instanceof FrameLayout error");
            FrameLayout layout = null;
            return layout;
        }
    }

    /**
     * change to RadioGroup.
     *
     * @param input input param
     * @return T.
     */
    public static <T> RadioGroup toRadioGroup(T input) {
        if (input instanceof RadioGroup) {
            return (RadioGroup) input;
        } else {
            LogUtil.info(TAG, "T instanceof SegmentedGroup error");
            RadioGroup segmentedGroup = null;
            return segmentedGroup;
        }
    }

    /**
     * change to RadioButton.
     *
     * @param input input param
     * @return T.
     */
    public static <T> RadioButton toRadioButton(T input) {
        if (input instanceof RadioButton) {
            return (RadioButton) input;
        } else {
            LogUtil.info(TAG, "T instanceof RadioButton error");
            RadioButton radioButton = null;
            return radioButton;
        }
    }

    /**
     * change to ArrayList.
     *
     * @param list input param
     * @return T.
     */
    public static <T> ArrayList<? extends Parcelable> toArrayList(T list) {
        if (list instanceof ArrayList) {
            return (ArrayList<? extends Parcelable>) list;
        } else {
            LogUtil.error(TAG, "T instanceof ArrayList<? extends Parcelable> error");
            ArrayList<? extends Parcelable> arrayList = null;
            return arrayList;
        }
    }
}
