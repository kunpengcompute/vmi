/*
 * 版权所有 (c) 华为技术有限公司 2021-2022
 * 功能说明: RGB转YUV函数 ARM64
 */

#if defined(__aarch64__)

.section .rodata
.align 4
.text

.macro CONVERT_PUSH_V_REGS
    stp    x19, x20, [sp, #-16]!
.endm

.macro CONVERT_POP_V_REGS
    ldp    x19, x20, [sp], #16
.endm

.macro RGB_TO_YUV_LINE16
    ld4         {v0.16b,v1.16b,v2.16b,v3.16b}, [x0], #64
    ld4         {v4.16b,v5.16b,v6.16b,v7.16b}, [x10], #64
    prfm        pldl1keep, [x0, 128]
    prfm        pldl1keep, [x10, 128]
    // UV add
    uaddlp      v28.8h, v0.16b // r
    uaddlp      v29.8h, v1.16b // g
    uaddlp      v30.8h, v2.16b // b
    uadalp      v28.8h, v4.16b
    uadalp      v29.8h, v5.16b
    uadalp      v30.8h, v6.16b
    urshr       v28.8h, v28.8h, #1 // DIV 2
    urshr       v29.8h, v29.8h, #1 // DIV 2
    urshr       v30.8h, v30.8h, #1 // DIV 2
    // Y clc
    umull       v3.8h,v16.8b,v0.8b
    umull2      v7.8h,v16.16b,v0.16b
    umlal       v3.8h,v17.8b,v1.8b
    umlal2      v7.8h,v17.16b,v1.16b
    umlal       v3.8h,v18.8b,v2.8b
    umlal2      v7.8h,v18.16b,v2.16b
    // Y clc
    umull       v26.8h,v16.8b,v4.8b
    umull2      v27.8h,v16.16b,v4.16b
    umlal       v26.8h,v17.8b,v5.8b
    umlal2      v27.8h,v17.16b,v5.16b
    umlal       v26.8h,v18.8b,v6.8b
    umlal2      v27.8h,v18.16b,v6.16b
    // UV clc
    mul         v0.8h, v30.8h, v20.8h
    mul         v1.8h, v28.8h, v20.8h
    mls         v0.8h, v29.8h, v21.8h
    mls         v1.8h, v29.8h, v24.8h
    mls         v0.8h, v28.8h, v22.8h
    mls         v1.8h, v30.8h, v23.8h
    add         v0.8h, v0.8h, v25.8h
    add         v1.8h, v1.8h, v25.8h
    uqshrn      v4.8b, v0.8h, #8
    uqshrn      v5.8b, v1.8h, #8
    st1         {v4.8b}, [x4], #8
    st1         {v5.8b}, [x6], #8
    uqrshrn     v28.8b, v3.8h, #8
    uqrshrn2    v28.16b, v7.8h, #8
    uqadd       v28.16b, v28.16b, v19.16b
    st1         {v28.16b}, [x2], #16
    uqrshrn     v6.8b, v26.8h, #8
    uqrshrn2    v6.16b, v27.8h, #8
    uqadd       v6.16b, v6.16b, v19.16b
    st1         {v6.16b}, [x11], #16
.endm

.macro RGB_TO_YUV_LINE8
    ld4         {v0.8b,v1.8b,v2.8b,v3.8b}, [x0], #32
    ld4         {v4.8b,v5.8b,v6.8b,v7.8b}, [x10], #32
    // UV add
    uaddlp      v28.4h, v0.8b // r
    uaddlp      v29.4h, v1.8b // g
    uaddlp      v30.4h, v2.8b // b
    uadalp      v28.4h, v4.8b
    uadalp      v29.4h, v5.8b
    uadalp      v30.4h, v6.8b
    urshr       v28.4h, v28.4h, #1 // DIV 2
    urshr       v29.4h, v29.4h, #1 // DIV 2
    urshr       v30.4h, v30.4h, #1 // DIV 2
    // Y clc
    umull       v3.8h,v16.8b,v0.8b
    umlal       v3.8h,v17.8b,v1.8b
    umlal       v3.8h,v18.8b,v2.8b
    // Y clc
    umull       v26.8h,v16.8b,v4.8b
    umlal       v26.8h,v17.8b,v5.8b
    umlal       v26.8h,v18.8b,v6.8b
    // UV clc
    mul         v0.4h, v30.4h, v20.4h // 56 * b
    mul         v1.4h, v28.4h, v20.4h // 56 * r
    mls         v0.4h, v29.4h, v21.4h // 37 * g
    mls         v1.4h, v29.4h, v24.4h // 47 * g
    mls         v0.4h, v28.4h, v22.4h // 19 * r
    mls         v1.4h, v30.4h, v23.4h //  9 * b

    add         v0.4h, v0.4h, v25.4h
    add         v1.4h, v1.4h, v25.4h
    uqshrn      v4.8b, v0.8h, #8
    uqshrn      v5.8b, v1.8h, #8
    
    st1         {v4.s}[0], [x4], x19
    st1         {v5.s}[0], [x6], x19
    uqrshrn     v28.8b, v3.8h, #8
    uqadd       v28.8b, v28.8b, v19.8b
    st1         {v28.8b}, [x2], #8
    uqrshrn     v6.8b, v26.8h, #8
    uqadd       v6.8b, v6.8b, v19.8b
    st1         {v6.8b}, [x11], #8
.endm

  .globl Rgb2YuvNeon64
Rgb2YuvNeon64:
    ldr         w15, [sp, 0]
    ldr         w13, [sp, 8]
    CONVERT_PUSH_V_REGS
    asr         x9, x15, #1
    add         x11, x2, x3    // second y line
    add         x12, x3, x3
    sub         x12, x12, x15
    sub         x5, x5, x9
    sub         x7, x7, x9
    mov         x19, #4
    movi        v16.16b, #66   // YR
    movi        v17.16b, #129  // YG
    movi        v18.16b, #25   // YB
    movi        v19.16b, #16
    movi        v20.8h, #56    // UVR
    movi        v21.8h, #37    // UG
    movi        v22.8h, #19    // UG
    movi        v23.8h, #9     // VB
    movi        v24.8h, #47    // VG
    movi        v25.16b, #0x80
    RGBToYUVColumnLoop:
        add         x10, x0, x1    // second rgb line
        mov         x14, x15 // loop x
        add         x20, x10, x1
        ABGRToYUVRowLoop:
            cmp         x14, #15
            b.lt        RGBToYUVRowLoopEnd
                RGB_TO_YUV_LINE16
            sub         x14, x14, #16
            b           ABGRToYUVRowLoop
        RGBToYUVRowLoopEnd:
        cmp         x14, #8
        b.ne        LINE_END
            RGB_TO_YUV_LINE8
    LINE_END:
        mov         x0, x20
        add         x2, x2, x12
        add         x11, x11, x12
        add         x4, x4, x5
        add         x6, x6, x7
        sub         x13, x13, #2
        cmp         x13, #0
        b.gt        RGBToYUVColumnLoop
    CONVERT_POP_V_REGS
    mov         x0, 0
    ret

#endif