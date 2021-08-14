/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.common;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.huawei.instructionstream.appui.R;
import com.huawei.instructionstream.appui.utils.InstanceofUtils;
import com.huawei.instructionstream.appui.utils.LogUtils;

/**
 * CommonDialog.
 *
 * @since 2018-07-05
 */
public class CommonDialog extends Dialog {
    /**
     * CommonDialog.
     *
     * @param context context.
     */
    public CommonDialog(Context context) {
        super(context);
    }

    /**
     * CommonDialog.
     *
     * @param context context.
     * @param themeResId res id.
     */
    public CommonDialog(Context context, int themeResId) {
        super(context, themeResId);
    }

    /**
     * Builder
     */
    public static class Builder {
        private static final String TAG = "CommonDialog";
        private Context context;
        private String title;
        private String message;
        private View contentView;
        private String positiveButtonText;
        private String negativeButtonText;
        private OnClickListener positiveButtonClickListener;
        private OnClickListener negativeButtonClickListener;

        public Builder(Context context) {
            this.context = context;
        }

        public Builder setTitle(String title) {
            this.title = title;
            return this;
        }

        public Builder setTitle(int title) {
            this.title = context.getString(title);
            return this;
        }

        public Builder setMessage(String message) {
            this.message = message;
            return this;
        }

        public Builder setMessage(int message) {
            this.message = context.getString(message);
            return this;
        }

        public Builder setContentView(View view) {
            this.contentView = view;
            return this;
        }

        /**
         * set positive button with string text.
         *
         * @param positiveButtonText the positive button text
         * @param listener the click listener
         * @return Builder.
         */
        public Builder setPositiveButton(String positiveButtonText, OnClickListener listener) {
            this.positiveButtonText = positiveButtonText;
            this.positiveButtonClickListener = listener;
            return this;
        }

        /**
         * set positive button with int text.
         *
         * @param positiveButtonText the positive button text
         * @param listener the click listener
         * @return Builder.
         */
        public Builder setPositiveButton(int positiveButtonText, OnClickListener listener) {
            this.positiveButtonText = context.getString(positiveButtonText);
            this.positiveButtonClickListener = listener;
            return this;
        }

        /**
         * set positive button with string text.
         *
         * @param positiveButtonText the positive button text
         * @param listener the click listener
         * @return Builder.
         */
        public Builder setNegativeButton(String negativeButtonText, OnClickListener listener) {
            this.negativeButtonText = negativeButtonText;
            this.negativeButtonClickListener = listener;
            return this;
        }

        /**
         * set positive button with int text.
         *
         * @param positiveButtonText the positive button text
         * @param listener the click listener
         * @return Builder.
         */
        public Builder setNegativeButton(int negativeButtonText, OnClickListener listener) {
            this.positiveButtonText = context.getString(negativeButtonText);
            this.negativeButtonClickListener = listener;
            return this;
        }

        /**
         * show dialog
         *
         * @return dialog.
         */
        public CommonDialog show() {
            final CommonDialog dialog = create();
            dialog.show();
            return dialog;
        }

        /**
         * set positive button.
         *
         * @param layout layout
         * @param dialog the dialog
         */
        public void setPositive(View layout, final CommonDialog dialog) {
            TextView positive = InstanceofUtils.toTextView(layout.findViewById(R.id.positive));
            if (positive !=null && positiveButtonText != null) {
                positive.setText(positiveButtonText);
                if (positiveButtonClickListener != null) {
                    positive.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            positiveButtonClickListener.onClick(dialog, DialogInterface.BUTTON_POSITIVE);
                        }
                    });
                }
            } else {
                positive.setVisibility(View.GONE);
            }
        }

        /**
         * set negative button.
         *
         * @param layout layout
         * @param dialog the dialog
         */
        public void setNegative(View layout, final CommonDialog dialog) {
            TextView negative = InstanceofUtils.toTextView(layout.findViewById(R.id.negative));
            if (negative != null && negativeButtonText != null) {
                negative.setText(negativeButtonText);
                if (negativeButtonClickListener != null) {
                    negative.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            negativeButtonClickListener.onClick(dialog, DialogInterface.BUTTON_NEGATIVE);
                        }
                    });
                }
            } else {
                negative.setVisibility(View.GONE);
            }
        }

        private LayoutInflater getLayoutInflater() {
            LayoutInflater inflater = null;
            if (context.getSystemService(Context.LAYOUT_INFLATER_SERVICE) instanceof LayoutInflater) {
                inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            }
            return inflater;
        }

        /**
         * create common diglog object.
         *
         * @return dialog object
         */
        public CommonDialog create() {
            final CommonDialog dialog = new CommonDialog(context, R.style.dialog);
            LayoutInflater inflater = getLayoutInflater();
            if (inflater == null){
                LogUtils.info(TAG, "inflater is null");
                return dialog;
            }
            View layout = inflater.inflate(R.layout.vmi_dialog_common, null);
            if (layout == null){
                LogUtils.info(TAG, "layout is null");
                return dialog;
            }
            dialog.addContentView(layout, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));
            TextView titleTex = InstanceofUtils.toTextView(layout.findViewById(R.id.title));
            titleTex.setText(title);

            setPositive(layout, dialog);
            setNegative(layout, dialog);

            TextView messageTV = InstanceofUtils.toTextView(layout.findViewById(R.id.message));
            LinearLayout content = null;
            if (layout.findViewById(R.id.content) instanceof LinearLayout) {
                content = (LinearLayout) layout.findViewById(R.id.content);
            }
            if (message != null && messageTV != null) {
                messageTV.setText(message);
            } else if (contentView != null && content != null) {
                // if no message set
                // add the contentView to the dialog body
                content.removeAllViews();
                content.addView(contentView, new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
            } else {
                LogUtils.info(TAG, "message is null, contentView is null");
            }

            dialog.setContentView(layout);

            return dialog;
        }
    }
}
