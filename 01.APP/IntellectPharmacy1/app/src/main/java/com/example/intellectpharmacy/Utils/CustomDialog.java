package com.example.intellectpharmacy.Utils;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.Gravity;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;

import com.example.intellectpharmacy.R;

public class CustomDialog extends Dialog {

    private TextView tvLoadingTx;
    private ImageView ivLoading;

    public CustomDialog(@NonNull Context context, String str) {
        super(context, R.style.loading_dialog);
        setCanceledOnTouchOutside(false);
        setOnCancelListener(new OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialogInterface) {
                dismiss();
            }
        });
        setContentView(R.layout.loading_dialog);
        tvLoadingTx = findViewById(R.id.tv_loading_tx);
        ivLoading = findViewById(R.id.iv_loading);
        tvLoadingTx.setText(str);
        Animation hyperspaceJumpAnimation = AnimationUtils.loadAnimation(
                context, R.anim.loading_animation
        );
        // 使用ImageView显示动画
        ivLoading.startAnimation(hyperspaceJumpAnimation);
        getWindow().getAttributes().gravity = Gravity.CENTER;//居中显示
        getWindow().getAttributes().dimAmount = 0.5f;//背景透明度  取值范围 0 ~ 1
    }

    public void dismiss() {
        super.dismiss();
    }
}
