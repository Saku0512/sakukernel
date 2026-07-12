# sakukernel

小さな 32bit x86 カーネル実験用リポジトリ。
今は Multiboot で起動し、VGA text mode に文字を出すところまで。

## ファイルの役割

- `src/boot.asm`: ブート直後の入口。スタックを用意して `kernel_main` を呼ぶ。
- `src/kernel.c`: C で書いたカーネル本体。VGA text mode へ文字を表示する。
- `linker.ld`: kernel.elf のメモリ配置を決めるリンカスクリプト。
- `Makefile`: ビルド、リンク、QEMU 起動の手順。

## Nix環境

環境に入る
```bash
nix develop
```

環境から出る
```bash
exit
```
