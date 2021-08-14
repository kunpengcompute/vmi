/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.common;

import android.app.Dialog;
import android.content.Context;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import com.huawei.instructionstream.appui.R;

/**
 * radio dialog
 *
 * @since 2020-03-31
 */
public class RadioDialog extends Dialog {
    /**
     * RadioDialog
     *
     * @param context Context
     */
    public RadioDialog(Context context) {
        super(context);
    }

    /**
     * RadioDialog
     *
     * @param context Context
     * @param theme   int
     */
    public RadioDialog(Context context, int theme) {
        super(context, theme);
    }

    /**
     * Builder
     */
    public static class Builder {
        private Context context;
        private String title;
        private View contentView;
        private String[] items;
        private int choosedIndex = 0;
        private OnDismissListener dismissListener;
        private OnRadioDialogItemSelectedListener onRadioDialogItemChoosedListener;

        /**
         * Builder
         *
         * @param context Context
         */
        public Builder(Context context) {
            this.context = context;
        }

        /**
         * Set the Dialog title from resource
         *
         * @param title int
         * @return Builder
         */
        public Builder setTitle(int title) {
            this.title = context.getText(title).toString();
            return this;
        }

        /**
         * Set the Dialog title from String
         *
         * @param title title
         * @return Builder
         */

        public Builder setTitle(String title) {
            this.title = title;
            return this;
        }

        /**
         * getItems
         *
         * @return String[]
         */
        public String[] getItems() {
            return items;
        }

        /**
         * setItems
         *
         * @param items String[]
         */
        public void setItems(String[] items) {
            this.items = items;
        }

        /**
         * getChoosedIndex
         *
         * @return int
         */
        public int getChoosedIndex() {
            return choosedIndex;
        }

        /**
         * setChoosedIndex
         *
         * @param choosedIndex int
         */
        public void setChoosedIndex(int choosedIndex) {
            this.choosedIndex = choosedIndex;
        }

        /**
         * setContentView
         *
         * @param view View
         * @return Builder
         */
        public Builder setContentView(View view) {
            this.contentView = view;
            return this;
        }

        /**
         * getDismissListener
         *
         * @return OnDismissListener
         */
        public OnDismissListener getDismissListener() {
            return dismissListener;
        }

        /**
         * setDismissListener
         *
         * @param dismissListener OnDismissListener
         */
        public void setDismissListener(OnDismissListener dismissListener) {
            this.dismissListener = dismissListener;
        }

        /**
         * getOnRadioDialogItemChoosedListener
         *
         * @return dialog
         */
        public OnRadioDialogItemSelectedListener getOnRadioDialogItemChoosedListener() {
            return onRadioDialogItemChoosedListener;
        }

        /**
         * setOnRadioDialogItemChoosedListener
         *
         * @param onRadioDialogItemChoosedListener dialog
         */
        public void setOnRadioDialogItemChoosedListener(
            OnRadioDialogItemSelectedListener onRadioDialogItemChoosedListener) {
            this.onRadioDialogItemChoosedListener = onRadioDialogItemChoosedListener;
        }

        /**
         * create
         *
         * @return RadioDialog
         */
        public RadioDialog create() {
            LayoutInflater inflater = null;
            if (context.getSystemService(Context.LAYOUT_INFLATER_SERVICE) instanceof LayoutInflater) {
                inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            }
            // instantiate the dialog with the custom Theme
            RadioDialog dialog = new RadioDialog(context, R.style.Dialog);
            View layout = inflater.inflate(R.layout.dialog_radio_layout, null);
            // set the dialog title
            View titleView = layout.findViewById(R.id.title);
            if (titleView instanceof TextView) {
                ((TextView) titleView).setText(title);
            }
            ListView listView = layout.findViewById(R.id.list);
            setListAdapter(listView, dialog);
            dialog.setContentView(layout);
            if (dismissListener != null) {
                dialog.setOnDismissListener(dismissListener);
            }
            WindowManager.LayoutParams params = dialog.getWindow().getAttributes();
            if (params != null) {
                params.gravity = Gravity.CENTER_HORIZONTAL | Gravity.BOTTOM;
                params.width = WindowManager.LayoutParams.MATCH_PARENT;
                params.height = WindowManager.LayoutParams.WRAP_CONTENT;
                params.verticalMargin = 0.1f;
                dialog.getWindow().setAttributes(params);
            }
            return dialog;
        }

        private void setListAdapter(ListView listView, final RadioDialog dialog) {
            listView.setAdapter(new BaseAdapter() {
                @Override
                public int getCount() {
                    return items.length;
                }

                @Override
                public Object getItem(int position) {
                    return items[position];
                }

                @Override
                public long getItemId(int position) {
                    return position;
                }

                @Override
                public View getView(final int position, View convertView, ViewGroup parent) {
                    LayoutInflater layoutInflater = LayoutInflater.from(context);
                    View itemView = layoutInflater.inflate(R.layout.radio_item_view, null);
                    View textView = itemView.findViewById(R.id.item_radio_text);
                    if (textView instanceof TextView) {
                        ((TextView) textView).setText(items[position]);
                    }
                    ImageView radioIcon = itemView.findViewById(R.id.item_radio_icon);
                    radioIcon.setImageResource(choosedIndex ==
                        position ? R.mipmap.btn_radio_active : R.mipmap.btn_radio_inactive);
                    itemView.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            if (onRadioDialogItemChoosedListener != null) {
                                onRadioDialogItemChoosedListener.onRadioDialogItemSelected(position);
                                dialog.dismiss();
                            }
                        }
                    });
                    return itemView;
                }
            });
        }
    }

    /**
     * OnRadioDialogItemSelectedListener
     */
    public interface OnRadioDialogItemSelectedListener {
        /**
         * onRadioDialogItemSelected
         *
         * @param position int
         */
        void onRadioDialogItemSelected(int position);
    }
}

